# 🔧 Troubleshooting - Sistema de Autorização

## Problemas Comuns e Soluções

### 1. Compilação

#### Erro: "auth-permission.hpp not found"
**Causa:** Diretório `src/core/auth/` não foi criado
**Solução:** Criar os diretórios manualmente ou usar:
```bash
mkdir -p src/core/auth
mkdir -p src/http/middlewares
mkdir -p src/http/controllers/auth
mkdir -p src/application/use-cases/login
mkdir -p src/application/use-cases/initialize
mkdir -p src/database/seeds
```

#### Erro: "Multiple definition of 'AuthContext'" ou conflitos de símbolos
**Causa:** Headers sendo incluídos múltiplas vezes ou sem include guards
**Solução:** Garantir que todos os `.hpp` têm `#pragma once` na primeira linha

#### Erro: "Undefined reference to 'LoginUseCase::LoginUseCase'"
**Causa:** Arquivo `.cpp` não está sendo compilado
**Solução:** Verificar CMakeLists.txt - adicionar os novos `.cpp`:
```cmake
file(GLOB_RECURSE SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
```
Deve incluir automaticamente todos os .cpp da pasta src/

#### Erro: "Undefined reference to 'InitializeAdminUseCase'"
**Causa:** Use case de inicialização não está linkada
**Solução:** Garantir que o arquivo está compilado:
```cpp
// Em app-context.cpp
#include "application/use-cases/initialize/initialize-admin-use-case.hpp"
```

---

### 2. Runtime - Banco de Dados

#### Erro: "role" column does not exist
**Causa:** Migration 005 não foi executada
**Solução:** 
1. Verificar se o arquivo `005_add_role_to_users.sql` existe
2. Se existe, deletar o banco e deixar rodar as migrations novamente
3. Ou executar manualmente:
```sql
ALTER TABLE users ADD COLUMN IF NOT EXISTS role VARCHAR(50) DEFAULT 'USER' NOT NULL;
```

#### Erro: "Duplicate key value violates unique constraint on email"
**Causa:** Email já existe no banco
**Solução:** 
1. Para testes, deletar a tabela users:
```sql
DELETE FROM users WHERE email = 'test@example.com';
```
2. Ou usar email diferente

#### Admin seed não foi criado
**Causa:** Migration 006 falhou ou seed já existe
**Solução:**
1. Verificar logs da aplicação
2. Executar manualmente:
```sql
INSERT INTO users (id, name, email, role, created_at)
VALUES ('admin-seed-001', 'Administrator', 'admin@reiment.local', 'ADMIN', CURRENT_TIMESTAMP)
ON CONFLICT DO NOTHING;
```

---

### 3. Runtime - Autenticação

#### Erro: "Authentication failed" ao chamar rota protegida
**Causa:** Header `x-user-id` não foi enviado ou está vazio
**Solução:** Incluir header em todas as requisições para rotas protegidas:
```bash
curl -X GET http://localhost:8080/api/users/me \
  -H "x-user-id: admin-seed-001"
```

#### Erro: "Invalid user_id" (404 on user fetch)
**Causa:** User ID no header não existe no banco
**Solução:**
1. Criar usuário primeiro:
```bash
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"name": "Test", "email": "test@example.com"}'
```
2. Use o `id` retornado no header

#### Erro: "403 Forbidden - Insufficient permissions"
**Causa:** User não tem permissão para aquela ação
**Solução:** 
1. Verificar role do usuário:
   - USER role: pode criar e ler trips/tasks
   - OWNER role: pode atualizar/deletar suas próprias trips
   - ADMIN role: pode fazer tudo
2. Se precisar testar como admin, use: `admin-seed-001`

---

### 4. Runtime - Permissões e Ownership

#### "You don't have permission to modify this resource" ao deletar
**Causa:** Você não é o owner do recurso
**Solução:**
1. Apenas owner ou admin pode deletar
2. Para testar como admin:
```bash
curl -X DELETE http://localhost:8080/api/trips/trip-id \
  -H "x-user-id: admin-seed-001"
```
3. Ou deletar com o user que criou:
```bash
curl -X DELETE http://localhost:8080/api/trips/trip-id \
  -H "x-user-id: user-id-que-criou"
```

#### Trip/Task criado com owner_user_id errado
**Causa:** Código antigo ainda está passando owner_user_id no request
**Solução:** 
1. Remover `owner_user_id` do JSON do request
2. O owner é sempre `authCtx->user_id`
3. Exemplo correto:
```bash
curl -X POST http://localhost:8080/api/trips \
  -H "x-user-id: user-123" \
  -H "Content-Type: application/json" \
  -d '{"name": "Minha Viagem"}'
# Owner será automaticamente user-123
```

---

### 5. Integração com Controllers

#### AuthContext está NULL/vazio
**Causa:** `AUTH_REQUIRE_AUTHENTICATED` não foi chamado
**Solução:** Sempre começar handlers com:
```cpp
void MyController::myMethod(...) {
	AUTH_REQUIRE_AUTHENTICATED(req, callback);  // ← OBRIGATÓRIO

	// Agora authCtx está disponível
	std::string user_id = authCtx->user_id;
}
```

#### Compilador não reconhece a macro AUTH_REQUIRE_*
**Causa:** Header não foi incluído
**Solução:** No topo do controller `.cpp`:
```cpp
#include "core/auth/auth-helpers.hpp"
```

#### `authCtx` diz "was not declared in this scope"
**Causa:** Macro não foi executada antes de usar
**Solução:** A macro declara `authCtx` automaticamente:
```cpp
AUTH_REQUIRE_AUTHENTICATED(req, callback);  // ← Declara authCtx
// authCtx agora está disponível
```

---

### 6. Testes

#### Postman: "Authentication failed"
**Solução:**
1. Adicionar header `x-user-id` em "Headers"
2. Usar um user_id válido que existe no banco
3. Testar primeiro com `admin-seed-001`

#### curl retorna erro 415 (Unsupported Media Type)
**Solução:** Adicionar header correto:
```bash
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \  # ← Obrigatório
  -d '{"name": "Test", "email": "test@example.com"}'
```

#### curl retorna erro 400 (Bad Request)
**Causa:** JSON inválido ou campo obrigatório faltando
**Solução:** 
1. Validar JSON syntax com: `json -verify "{}"`
2. Verificar campos obrigatórios na documentação
3. Ver mensagem de erro retornada

#### Response vindo vazia (204 No Content)
**Esperado:** DELETE retorna 204 com body vazio
**Solução:** Isso é correto! 204 significa sucesso sem dados

---

### 7. Integração com Firebase/Supabase (Futuro)

#### Como migrar do header simples para JWT?
**Passos:**
1. Criar novo middleware `firebase-auth-middleware.hpp`
2. Validar token JWT em vez de header simples
3. Manter mesmo `AuthContext`
4. Manter mesmas macros e funções

Exemplo:
```cpp
// firebase-auth-middleware.hpp
class FirebaseAuthMiddleware {
	static std::optional<AuthContext> loadFromToken(
		const drogon::HttpRequestPtr& req
	) {
		auto token = extractBearerToken(req);
		auto firebaseUser = firebaseAuth.verifyIdToken(token);
		auto dbUser = userRepository.findByEmail(firebaseUser.email);
		return AuthContext(...);
	}
};
```

---

### 8. Performance

#### Queries lentas ao carregar authCtx
**Causa:** Falta de índice na coluna `id` ou `email`
**Solução:** Migrations já têm índices. Se tiver problema:
```sql
CREATE INDEX idx_users_id ON users(id);
CREATE INDEX idx_users_email ON users(email);
```

#### Muitas queries ao carregar permissões
**Otimização:** AuthContext calcula permissões localmente em memória
- Não precisa consultar banco a cada permissão
- Usa `getDefaultPermissions()` local

---

### 9. Segurança

#### User consegue modificar dados de outro user
**Solução implementada:** 
1. Todos os updates usam `authCtx->user_id`
2. Controllers validam ownership
3. Auth é obrigatório

#### Admin consegue fazer tudo?
**Sim, e é esperado:**
```cpp
AUTH_REQUIRE_OWNERSHIP(authCtx, owner_id, callback);
// Isso permite admin (isAdmin() retorna true)
```

Se quiser restringir admin:
```cpp
if (authCtx->user_id != owner_id) {  // Sem permitir admin
	callback(responses::forbidden("Only owner can modify"));
	return;
}
```

---

## Checklist de Debuggagem

- [ ] Arquivo `.cpp` está em `src/` e será compilado?
- [ ] Header `.hpp` tem `#pragma once`?
- [ ] Include path está correto (sem ../ desnecessários)?
- [ ] Campo `role` existe na tabela `users`?
- [ ] Admin seed foi criado (`admin-seed-001`)?
- [ ] Request tem header `x-user-id` para rotas protegidas?
- [ ] User ID no header existe no banco?
- [ ] JSON é válido (testar em json validator)?
- [ ] Content-Type header está correto?
- [ ] Macro `AUTH_REQUIRE_*` foi chamada antes de usar `authCtx`?
- [ ] Controllers incluem `auth-helpers.hpp`?
- [ ] Banco está rodando e acessível?

## Logs e Debugging

Para adicionar logs:
```cpp
#include <iostream>

std::cout << "User ID: " << authCtx->user_id << std::endl;
std::cout << "Role: " << auth::roleToString(authCtx->role) << std::endl;
std::cout << "Permissions count: " << authCtx->permissions.size() << std::endl;
```

## Contato / Ajuda

Se tiver problema:
1. Verificar este documento
2. Revisar logs da aplicação
3. Consultar `AUTHORIZATION.md` para detalhes técnicos
4. Consultar `TESTING.md` para exemplos de uso
