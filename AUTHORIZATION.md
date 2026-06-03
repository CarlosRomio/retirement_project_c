# 🔒 Guia de Autorização e Proteção de Rotas

## Visão Geral

O sistema de autorização está organizado em camadas:

1. **Auth Layer** (`src/core/auth/`) - Enums de roles, permissions e helpers
2. **Validators** - Validação de autenticação e permissões
3. **Controllers** - Uso de macros para proteção de rotas
4. **Repositories** - Verificação de ownership de recursos

## Arquitetura

### Roles Disponíveis

```cpp
enum class Role {
	USER,   // Usuário comum (pode criar viagens e tarefas)
	OWNER,  // Proprietário (pode gerenciar suas viagens)
	ADMIN   // Administrador (acesso total)
};
```

### Permissões

Cada role tem um conjunto de permissões padrão:

- **USER**: CREATE_USER, UPDATE_OWN_USER, DELETE_OWN_USER, CREATE_TRIP, READ_TRIP, CREATE_TASK, READ_TASK, MARK_TASK_COMPLETE
- **OWNER**: Tudo do USER + UPDATE_TRIP, DELETE_TRIP, ADD_TRIP_PARTICIPANT, REMOVE_TRIP_PARTICIPANT, UPDATE_TASK, DELETE_TASK
- **ADMIN**: Tudo + MANAGE_USERS, MANAGE_ROLES, VIEW_LOGS, ADMIN_ACCESS

### Como Usar nos Controllers

#### 1. Requer Autenticação Básica

```cpp
void MyController::someMethod(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	// Carrega contexto de autenticação
	AUTH_REQUIRE_AUTHENTICATED(req, callback);

	// Agora você tem acesso a authCtx
	std::cout << authCtx->user_id << std::endl;
}
```

#### 2. Requer Permissão Específica

```cpp
void MyController::createTrip(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	AUTH_REQUIRE_AUTHENTICATED(req, callback);
	AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::CREATE_TRIP, callback);

	// Código da rota...
}
```

#### 3. Requer Ownership (Admin ou Owner)

```cpp
void MyController::deleteTrip(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback,
	const std::string& trip_id
) {
	AUTH_REQUIRE_AUTHENTICATED(req, callback);
	AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::DELETE_TRIP, callback);

	// Busca o recurso
	auto tripOpt = context.getTripUseCase.execute(trip_id);
	if (!tripOpt) {
		callback(responses::notFound("Trip not found"));
		return;
	}

	// Valida ownership
	AUTH_REQUIRE_OWNERSHIP(authCtx, tripOpt->owner_user_id, callback);

	// Código da rota...
}
```

#### 4. Requer Admin

```cpp
void MyController::deleteUser(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback,
	const std::string& user_id
) {
	AUTH_REQUIRE_AUTHENTICATED(req, callback);
	AUTH_REQUIRE_ADMIN(authCtx, callback);

	// Apenas admins chegam aqui
}
```

## Fluxo de Autenticação

1. **Request chega** com header `x-user-id`
2. **AUTH_REQUIRE_AUTHENTICATED** macro:
   - Extrai `x-user-id` do header
   - Busca usuário no banco com suas permissões
   - Cria `AuthContext` com dados do usuário e permissions
3. **Validações adicionais** (permissão, ownership, admin)
4. **Se válido**, continua com `authCtx` disponível
5. **Se inválido**, retorna erro 401 ou 403

## Como Integrar com Firebase/Supabase

Quando migrar para Firebase/Supabase:

1. **Criar novo middleware** que valida token JWT
2. **Substituir extraction** do header `x-user-id` por validação JWT
3. **Manter mesmo `AuthContext`** e permissões
4. **Tabela `users`** continua com roles locais

Exemplo:

```cpp
// auth/firebase-auth-middleware.hpp
class FirebaseAuthMiddleware {
	static std::optional<AuthContext> loadFromToken(const drogon::HttpRequestPtr& req) {
		auto token = extractTokenFromHeader(req);  // Bearer token

		// Validar com Firebase
		auto firebaseUser = firebaseAuth.verifyIdToken(token);

		if (!firebaseUser) return std::nullopt;

		// Buscar role local
		auto dbUser = userRepository.findByEmail(firebaseUser.email);

		return AuthContext(
			dbUser.id,
			dbUser.email,
			dbUser.name,
			dbUser.role
		);
	}
};
```

## Rotas Públicas vs Protegidas

### Públicas (Sem autenticação)

```
POST   /api/users              - Criar novo usuário
POST   /api/auth/login         - Login
GET    /health                 - Health check
GET    /readiness              - Readiness check
```

### Protegidas (Requer autenticação)

```
GET    /api/users/me           - Ver perfil próprio
PUT    /api/users/me           - Atualizar perfil próprio

GET    /api/trips/:id          - Ver viagem (se membro)
POST   /api/trips              - Criar viagem
DELETE /api/trips/:id          - Deletar viagem (owner only)

GET    /api/tasks/:id          - Ver tarefa (se membro)
POST   /api/tasks              - Criar tarefa
DELETE /api/tasks/:id          - Deletar tarefa (owner only)
```

## Estrutura de Arquivos

```
src/core/auth/
├── auth-permission.hpp         # Enums Role/Permission, getDefaultPermissions()
├── auth-validator.hpp          # Validação de permissões
├── auth-context-loader.hpp     # Carrega contexto do banco
├── auth-helpers.hpp            # Macros AUTH_REQUIRE_*
└── auth-extractor.hpp          # Extract x-user-id (será substituído)

src/http/middlewares/
├── auth-context-middleware.hpp # Middleware Drogon
└── auth-context-middleware.cpp

src/http/controllers/
├── auth/
│   ├── auth-controller.hpp     # Login endpoint
│   └── auth-controller.cpp
├── user/
├── trip/
└── task/
```

## Exemplos Completos

### Criar Viagem (Autenticado)

```cpp
void TripController::create(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	try {
		AUTH_REQUIRE_AUTHENTICATED(req, callback);
		AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::CREATE_TRIP, callback);

		auto json = req->getJsonObject();
		if (!json) {
			callback(responses::badRequest("Invalid JSON"));
			return;
		}

		// ... validações ...

		CreateTripInput input;
		input.name = name;
		input.owner_user_id = authCtx->user_id;  // ← Use user_id do contexto

		auto result = context.createTripUseCase.execute(input);
		// ... resposta ...
	}
	catch (const std::exception& e) {
		callback(responses::internalServerError());
	}
}
```

### Deletar Viagem (Owner Only)

```cpp
void TripController::remove(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback,
	const std::string& id
) {
	try {
		AUTH_REQUIRE_AUTHENTICATED(req, callback);
		AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::DELETE_TRIP, callback);

		// Busca recurso
		auto tripOpt = context.getTripUseCase.execute(id);
		if (!tripOpt) {
			callback(responses::notFound("Trip not found"));
			return;
		}

		// Valida ownership (ou admin)
		AUTH_REQUIRE_OWNERSHIP(authCtx, tripOpt->owner_user_id, callback);

		// Deleta
		const auto removed = context.deleteTripUseCase.execute(id);

		auto response = drogon::HttpResponse::newHttpResponse();
		response->setStatusCode(drogon::k204NoContent);
		callback(response);
	}
	catch (const std::exception&) {
		callback(responses::internalServerError());
	}
}
```

## Códigos de Erro

- **401 Unauthorized** - Falta autenticação (header `x-user-id` ausente ou inválido)
- **403 Forbidden** - Autenticado mas sem permissão para aquela ação
- **404 NotFound** - Recurso não existe

## Próximos Passos

1. ✅ Sistema de autorização implementado
2. ⏳ Integração com Firebase/Supabase (substituir header validation)
3. ⏳ Rate limiting por usuário
4. ⏳ Audit logging de ações
5. ⏳ Soft deletes com auditoria
