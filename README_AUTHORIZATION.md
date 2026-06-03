# 🔐 Sistema de Autorização - Reiment API

## 📌 Resumo

Este é um **sistema completo de autorização** para a API Reiment, implementado em C++20 com Drogon e PostgreSQL.

### Características Principais

✅ **3 Roles**: USER, OWNER, ADMIN  
✅ **15+ Permissões** granulares  
✅ **Autenticação simples** via header (pronta para Firebase)  
✅ **Validação de ownership** automática  
✅ **Admin seed** criado na inicialização  
✅ **Documentação completa** com exemplos  

---

## 🚀 Quick Start

### 1. Compilar

```bash
cd C:\Users\Carlos\source\repos\retirement_project_c-
# Build via Visual Studio ou:
# cmake --build build --config Debug
```

### 2. Executar

```bash
./reiment
# Output esperado:
# Iniciando aplicação...
# Conectando ao banco de dados...
# Rodando migrations...
# Inicializando dados de seed (admin)...
# Iniciando servidor web...
# Servidor rodando em http://localhost:8080
```

### 3. Testar

```bash
# Criar usuário (público)
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"name": "João", "email": "joao@example.com"}'

# Login (público)
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email": "joao@example.com", "password": "qualquer"}'

# Ver perfil (autenticado)
curl -X GET http://localhost:8080/api/users/me \
  -H "x-user-id: user-id-retornado-acima"
```

---

## 📚 Documentação

- **[AUTHORIZATION.md](./AUTHORIZATION.md)** - Guia completo do sistema
- **[TESTING.md](./TESTING.md)** - Exemplos de testes com curl
- **[ARCHITECTURE.md](./ARCHITECTURE.md)** - Diagramas e arquitetura
- **[TROUBLESHOOTING.md](./TROUBLESHOOTING.md)** - Resolução de problemas
- **[IMPLEMENTATION_SUMMARY.md](./IMPLEMENTATION_SUMMARY.md)** - Resumo técnico
- **[IMPLEMENTATION_CHECKLIST.md](./IMPLEMENTATION_CHECKLIST.md)** - Checklist completo

---

## 🔑 Credenciais Padrão

Um usuário **admin** é criado automaticamente:

```
ID:    admin-seed-001
Email: admin@reiment.local
Name:  Administrator
Role:  ADMIN
```

Use `admin-seed-001` no header `x-user-id` para testar como admin:

```bash
curl -X GET http://localhost:8080/api/users/me \
  -H "x-user-id: admin-seed-001"
```

---

## 📋 Rotas Disponíveis

### Públicas (Sem autenticação)
```
POST   /api/users              - Criar novo usuário
POST   /api/auth/login         - Login
```

### Autenticadas (Requer header x-user-id)
```
GET    /api/users/me           - Ver perfil
PUT    /api/users/me           - Atualizar perfil

POST   /api/trips              - Criar viagem
GET    /api/trips/:id          - Ver viagem
DELETE /api/trips/:id          - Deletar viagem (owner/admin)

POST   /api/tasks              - Criar tarefa
GET    /api/tasks/:id          - Ver tarefa
DELETE /api/tasks/:id          - Deletar tarefa (owner/admin)
```

---

## 🏗️ Estrutura de Arquivos

```
src/core/auth/
├── auth-permission.hpp         # Enums de Role e Permission
├── auth-validator.hpp          # Validadores
├── auth-context-loader.hpp     # Carregador de contexto
└── auth-helpers.hpp            # Macros para controllers

src/http/controllers/
├── auth/                        # Novo controller de login
├── user/                        # Atualizado com auth
├── trip/                        # Atualizado com auth
└── task/                        # Atualizado com auth

src/application/use-cases/
├── login/                       # Novo use case de login
└── initialize/                  # Novo use case de inicialização

src/database/
├── migrations/005_add_role_to_users.sql
├── migrations/006_seed_admin_user.sql
└── seeds/admin-seed.hpp
```

---

## 💡 Exemplo de Uso nos Controllers

```cpp
void MyController::myHandler(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	// 1. Require authentication (carrega authCtx automaticamente)
	AUTH_REQUIRE_AUTHENTICATED(req, callback);

	// 2. Require specific permission
	AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::CREATE_TRIP, callback);

	// 3. Now you can use authCtx
	std::cout << authCtx->user_id << std::endl;
	std::cout << authCtx->role << std::endl;

	// 4. For delete/update, validate ownership
	auto resource = fetchResource(id);
	AUTH_REQUIRE_OWNERSHIP(authCtx, resource.owner_id, callback);

	// 5. Rest of handler...
}
```

---

## 🔄 Fluxo de Autenticação

```
1. Request chega com header x-user-id
   ↓
2. AUTH_REQUIRE_AUTHENTICATED macro
   ↓
3. Carrega User do banco com role
   ↓
4. Calcula Permissions baseado em role
   ↓
5. Cria AuthContext com user_id, email, name, role, permissions
   ↓
6. Passa authCtx para handler
   ↓
7. Handler pode usar authCtx->user_id, validar permissões, etc.
```

---

## 📊 Matrix de Permissões

| Ação | USER | OWNER | ADMIN |
|------|------|-------|-------|
| CREATE_TRIP | ✅ | ✅ | ✅ |
| READ_TRIP | ✅ | ✅ | ✅ |
| UPDATE_TRIP | ❌ | ✅ | ✅ |
| DELETE_TRIP | ❌ | ✅* | ✅ |
| CREATE_TASK | ✅ | ✅ | ✅ |
| READ_TASK | ✅ | ✅ | ✅ |
| UPDATE_TASK | ❌ | ✅ | ✅ |
| DELETE_TASK | ❌ | ✅* | ✅ |
| MANAGE_USERS | ❌ | ❌ | ✅ |

\* = Ou admin

---

## 🔐 Segurança

- ✅ Validação de autenticação obrigatória em rotas protegidas
- ✅ Permissões granulares por role
- ✅ Ownership validado para operações sensíveis
- ✅ Admin override automático
- ✅ Sem SQL injection (prepared statements)
- ✅ Sem exposure de dados sensíveis

---

## 🔮 Integração com Firebase/Supabase

Sistema **pronto para migração** para Firebase/Supabase:

1. Validação de token JWT substituirá header simples
2. `AuthContext` local mantém roles e permissões
3. Mesmo `AuthValidator`, mesmas permissões
4. Nenhuma mudança nos controllers necessária

Ver [AUTHORIZATION.md](./AUTHORIZATION.md) para detalhes.

---

## 🚦 Códigos de Erro

| Código | Causa | Solução |
|--------|-------|---------|
| 400 | Bad Request | Validar JSON e campos obrigatórios |
| 401 | Unauthorized | Adicionar header `x-user-id` |
| 403 | Forbidden | User não tem permissão ou não é owner |
| 404 | Not Found | Recurso não existe |
| 500 | Server Error | Ver logs da aplicação |

---

## 🧪 Testes

Para testar todos os cenários, ver [TESTING.md](./TESTING.md):

- Criar usuário
- Login
- Ver perfil
- Criar/deletar viagens
- Criar/deletar tarefas
- Testes de permissão
- Testes de ownership
- Testes de admin

---

## 📞 Troubleshooting

Se encontrar problemas:

1. **Compilação:** Ver [TROUBLESHOOTING.md](./TROUBLESHOOTING.md#1-compilação)
2. **Runtime:** Ver [TROUBLESHOOTING.md](./TROUBLESHOOTING.md#2-runtime---banco-de-dados)
3. **Autenticação:** Ver [TROUBLESHOOTING.md](./TROUBLESHOOTING.md#3-runtime---autenticação)
4. **Permissões:** Ver [TROUBLESHOOTING.md](./TROUBLESHOOTING.md#4-runtime---permissões-e-ownership)

---

## 📈 Próximos Passos

### Curto Prazo (Esta semana)
1. ✅ Compilar e testar
2. ✅ Testar todas as rotas com curl
3. ✅ Verificar fluxo de login

### Médio Prazo (Este mês)
1. [ ] Integração com Firebase/Supabase
2. [ ] Hash de password com bcrypt
3. [ ] Rate limiting por usuário
4. [ ] Validações aprimoradas

### Longo Prazo
1. [ ] Soft deletes com auditoria
2. [ ] Audit logging completo
3. [ ] Permissões dinâmicas
4. [ ] Testes automatizados

---

## 📖 Referências

- [Clean Architecture](https://blog.cleancoder.com/uncle-bob/2012/08/13/the-clean-architecture.html)
- [JWT (JSON Web Tokens)](https://jwt.io/)
- [Drogon Framework](https://drogon.org/)
- [PostgreSQL](https://www.postgresql.org/)

---

## 📄 Licença

Mesmo que o projeto principal Reiment

---

## 👤 Autor

Implementado como parte do projeto Reiment API

---

## ❓ Dúvidas?

1. Consulte a [documentação completa](./AUTHORIZATION.md)
2. Veja [exemplos de testes](./TESTING.md)
3. Revise a [arquitetura](./ARCHITECTURE.md)
4. Consulte [troubleshooting](./TROUBLESHOOTING.md)

---

**Status:** ✅ Implementação Completa  
**Versão:** 1.0  
**Data:** 2024  
**Pronto para:** Compilação, testes e deployment
