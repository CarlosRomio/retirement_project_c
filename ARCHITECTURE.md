# 🏗️ Arquitetura do Sistema de Autorização

## Diagrama de Fluxo - Request com Autenticação

```
┌─────────────────────────────────────────────────────────────────┐
│ CLIENT REQUEST                                                  │
│ GET /api/users/me                                               │
│ Header: x-user-id: user-123                                     │
└────────────────────────────┬────────────────────────────────────┘
							 │
							 ▼
		 ┌───────────────────────────────────────┐
		 │ DROGON HTTP SERVER                    │
		 │ Recebe requisição                     │
		 └───────────────┬───────────────────────┘
						 │
						 ▼
		 ┌───────────────────────────────────────┐
		 │ USER CONTROLLER                       │
		 │ me() handler                          │
		 └───────────────┬───────────────────────┘
						 │
						 ▼
	┌────────────────────────────────────────────┐
	│ AUTH_REQUIRE_AUTHENTICATED(req, callback)  │
	│ MACRO - Linha 1 do handler                 │
	└────────────┬───────────────────────────────┘
				 │
				 ▼
	┌────────────────────────────────────────────┐
	│ loadAuthContextFromRequest(req)            │
	│ Carrega contexto completo                  │
	└────────────┬───────────────────────────────┘
				 │
				 ├─────────────────────────────┐
				 │ AuthValidator::extractUserId│
				 │ Lê header x-user-id         │
				 └────────────┬────────────────┘
							  │
							  ▼
					┌──────────────────────┐
					│ "user-123"           │
					│ User ID extraído     │
					└────────────┬─────────┘
								 │
								 ▼
			┌────────────────────────────────────┐
			│ userRepository.findById("user-123")│
			│ Busca user no banco                │
			└────────────┬───────────────────────┘
						 │
						 ▼
		┌─────────────────────────────────────┐
		│ PostgreSQL                          │
		│ SELECT * FROM users WHERE id=...   │
		└────────────┬────────────────────────┘
					 │
					 ▼
		┌─────────────────────────────────────┐
		│ User encontrado                     │
		│ {                                   │
		│   id: "user-123",                   │
		│   name: "João",                     │
		│   email: "joao@example.com",        │
		│   role: "USER"                      │
		│ }                                   │
		└────────────┬────────────────────────┘
					 │
					 ▼
		┌─────────────────────────────────────┐
		│ AuthContext criado                  │
		│ {                                   │
		│   user_id: "user-123",              │
		│   email: "joao@example.com",        │
		│   name: "João",                     │
		│   role: Role::USER,                 │
		│   permissions: [...]                │
		│ }                                   │
		└────────────┬────────────────────────┘
					 │
					 ▼
		┌─────────────────────────────────────┐
		│ getDefaultPermissions(Role::USER)   │
		│ Carrega permissões padrão           │
		└────────────┬────────────────────────┘
					 │
					 ▼
		┌─────────────────────────────────────┐
		│ AuthContext retornado ao controller │
		│ authCtx disponível para use         │
		└────────────┬────────────────────────┘
					 │
					 ▼
		┌─────────────────────────────────────┐
		│ Handler continua com authCtx        │
		│ // Código do controller             │
		│ auto user = ...findById(authCtx...);│
		└────────────┬────────────────────────┘
					 │
					 ▼
		┌─────────────────────────────────────┐
		│ Retorna 200 OK com dados do user    │
		│ {                                   │
		│   id: "user-123",                   │
		│   name: "João",                     │
		│   email: "joao@example.com",        │
		│   role: "USER"                      │
		│ }                                   │
		└─────────────────────────────────────┘
```

## Diagrama de Classes

```
┌─────────────────────────┐
│ AuthContext             │
├─────────────────────────┤
│ - user_id: string       │
│ - email: string         │
│ - name: string          │
│ - role: Role            │
│ - permissions: vector   │
├─────────────────────────┤
│ + hasPermission()       │
│ + isAdmin()             │
│ + isOwner()             │
└─────────────────────────┘
		△ uses
		│
		│
┌───────┴──────────────────────────────┐
│ AuthValidator                        │
├──────────────────────────────────────┤
│ + extractUserId()                    │
│ + requireAuthentication()            │
│ + requirePermission()                │
│ + requireOwnership()                 │
│ + requireAdmin()                     │
│ + requireAllPermissions()            │
│ + requireAnyPermission()             │
└──────────────────────────────────────┘


┌──────────────────────────────────────────┐
│ AuthContextLoader                        │
├──────────────────────────────────────────┤
│ + loadFullContext(req, userFinder)      │
│   Returns: optional<AuthContext>        │
└──────────────────────────────────────────┘
		△ uses
		│
		│
┌───────┴─────────────────────┐
│ IUserRepository             │
├─────────────────────────────┤
│ + save()                    │
│ + findById()                │
│ + findByEmail()             │
│ + update()                  │
│ + emailExists()             │
└─────────────────────────────┘
		△ implements
		│
		│
┌───────┴──────────────────────────────────┐
│ PostgresUserRepository                   │
├──────────────────────────────────────────┤
│ + save() { /* implementation */ }        │
│ + findById() { /* implementation */ }    │
│ + findByEmail() { /* implementation */ } │
│ + update() { /* implementation */ }      │
│ + emailExists() { /* implementation */ } │
└──────────────────────────────────────────┘
```

## Diagrama de Permissões

```
┌──────────────┐
│ Role::ADMIN  │ ──► Todas as 20+ permissões
└──────────────┘

┌──────────────┐
│ Role::OWNER  │ ──► CREATE_USER, UPDATE_OWN_USER, DELETE_OWN_USER,
└──────────────┘     READ_USER, CREATE_TRIP, READ_TRIP, UPDATE_TRIP,
					 DELETE_TRIP, ADD_TRIP_PARTICIPANT,
					 REMOVE_TRIP_PARTICIPANT, CREATE_TASK, READ_TASK,
					 UPDATE_TASK, DELETE_TASK, MARK_TASK_COMPLETE

┌──────────────┐
│ Role::USER   │ ──► CREATE_USER, UPDATE_OWN_USER, DELETE_OWN_USER,
└──────────────┘     READ_USER, CREATE_TRIP, READ_TRIP, CREATE_TASK,
					 READ_TASK, MARK_TASK_COMPLETE
```

## Diagrama de Controllers e Rotas

```
┌─────────────────────────────────────────────────────────────────────┐
│ ROTAS PÚBLICAS                                                      │
├─────────────────────────────────────────────────────────────────────┤
│ POST   /api/users              → UserController::create()           │
│ POST   /api/auth/login         → AuthController::login()            │
│ GET    /health                 → HealthController::health()         │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────┐
│ ROTAS AUTENTICADAS - USER                                           │
├─────────────────────────────────────────────────────────────────────┤
│ GET    /api/users/me           → UserController::me()               │
│        Requer: AUTH                                                 │
│                                                                     │
│ PUT    /api/users/me           → UserController::updateMe()         │
│        Requer: AUTH + UPDATE_OWN_USER                              │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────┐
│ ROTAS AUTENTICADAS - TRIPS                                          │
├─────────────────────────────────────────────────────────────────────┤
│ POST   /api/trips              → TripController::create()           │
│        Requer: AUTH + CREATE_TRIP                                  │
│        Owner: authCtx->user_id                                      │
│                                                                     │
│ GET    /api/trips/:id          → TripController::getById()          │
│        Requer: AUTH + READ_TRIP                                    │
│                                                                     │
│ DELETE /api/trips/:id          → TripController::remove()           │
│        Requer: AUTH + DELETE_TRIP + OWNERSHIP                      │
│        (ou ADMIN)                                                   │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────┐
│ ROTAS AUTENTICADAS - TASKS                                          │
├─────────────────────────────────────────────────────────────────────┤
│ POST   /api/tasks              → TaskController::create()           │
│        Requer: AUTH + CREATE_TASK                                  │
│        Owner: authCtx->user_id                                      │
│                                                                     │
│ GET    /api/tasks/:id          → TaskController::getById()          │
│        Requer: AUTH + READ_TASK                                    │
│                                                                     │
│ DELETE /api/tasks/:id          → TaskController::remove()           │
│        Requer: AUTH + DELETE_TASK + OWNERSHIP                      │
│        (ou ADMIN)                                                   │
└─────────────────────────────────────────────────────────────────────┘
```

## Diagrama de Banco de Dados

```
┌──────────────────────────────────────────────────────────┐
│ users                                                    │
├──────────────────────────────────────────────────────────┤
│ id           TEXT PRIMARY KEY                            │
│ name         TEXT NOT NULL                               │
│ email        TEXT UNIQUE NOT NULL                        │
│ role         VARCHAR(50) NOT NULL DEFAULT 'USER'  ← NEW │
│ password_hash VARCHAR(255)                        ← NEW │
│ created_at   TIMESTAMP DEFAULT CURRENT_TIMESTAMP        │
├──────────────────────────────────────────────────────────┤
│ Índices:                                                 │
│ - PRIMARY KEY (id)                                       │
│ - UNIQUE (email)                                         │
│ - INDEX (role)  ← NEW                                   │
└──────────────────────────────────────────────────────────┘

Admin Seed:
┌──────────────────────────────────────────────────────────┐
│ id: admin-seed-001                                       │
│ name: Administrator                                      │
│ email: admin@reiment.local                               │
│ role: ADMIN                                              │
│ password_hash: NULL (por enquanto)                       │
└──────────────────────────────────────────────────────────┘
```

## Fluxo de Ownership Check

```
┌─────────────────────────────────────────┐
│ DELETE /api/trips/:trip_id              │
│ Header: x-user-id: user-123             │
└────────────────┬────────────────────────┘
				 │
				 ▼
┌─────────────────────────────────────────┐
│ AUTH_REQUIRE_AUTHENTICATED              │
│ Carrega authCtx para user-123           │
└────────────────┬────────────────────────┘
				 │
				 ▼
┌─────────────────────────────────────────┐
│ AUTH_REQUIRE_PERMISSION (DELETE_TRIP)   │
│ user-123 tem permissão?                 │
│ (USER role não tem, OWNER tem, ADMIN tem)
└────────────────┬────────────────────────┘
				 │
				 ▼
		┌────────────────────┐
		│ tripRepository     │
		│ .getTripUseCase    │
		│ .execute(trip_id)  │
		│ Busca a viagem     │
		└────────┬───────────┘
				 │
				 ▼
		┌────────────────────────────────┐
		│ Trip encontrada                │
		│ owner_user_id: user-456        │
		└────────────┬───────────────────┘
					 │
					 ▼
		┌──────────────────────────────────────────┐
		│ AUTH_REQUIRE_OWNERSHIP(authCtx, user-456)│
		│                                          │
		│ if (user-123 == user-456) OK ✓          │
		│    (Owner consegue deletar)              │
		│                                          │
		│ if (user-123 != user-456 && isAdmin) ✓  │
		│    (Admin consegue deletar)              │
		│                                          │
		│ if (user-123 != user-456 && !isAdmin) ✗ │
		│    (Usuário comum não consegue)          │
		│    Return 403 Forbidden                  │
		└──────────────────────────────────────────┘
```

## Ordem de Execução de Macros

```
Handler({
	┌─────────────────────────────────────────┐ Step 1
	│ AUTH_REQUIRE_AUTHENTICATED(req, callback)│ Autentica
	└────────────┬────────────────────────────┘
				 │
	┌────────────▼────────────────────────────┐ Step 2
	│ AUTH_REQUIRE_PERMISSION(authCtx, perm)  │ Valida permissão
	└────────────┬────────────────────────────┘
				 │
	┌────────────▼────────────────────────────┐ Step 3
	│ Auto carrega recurso do banco           │ Busca recurso
	└────────────┬────────────────────────────┘
				 │
	┌────────────▼────────────────────────────┐ Step 4
	│ AUTH_REQUIRE_OWNERSHIP(authCtx, owner)  │ Valida ownership
	└────────────┬────────────────────────────┘
				 │
	┌────────────▼────────────────────────────┐ Step 5
	│ Executa lógica do handler               │ Faz a ação
	└────────────┬────────────────────────────┘
				 │
	┌────────────▼────────────────────────────┐ Step 6
	│ Retorna resposta 200/201/204/...        │ Sucesso
	└─────────────────────────────────────────┘

Se falhar em qualquer step → Return erro (401/403/404)
})
```

## Integração Futura com Firebase

```
┌─────────────────────────────────────────────────────────┐
│ HOJE - Autenticação Local (x-user-id header)            │
├─────────────────────────────────────────────────────────┤
│ AuthValidator::extractUserId() ← Header simples         │
│ AuthContextLoader::loadFullContext() ← Busca no banco   │
└─────────────────────────────────────────────────────────┘

						  ↓ Migração

┌─────────────────────────────────────────────────────────┐
│ FUTURO - Autenticação Firebase (JWT token)              │
├─────────────────────────────────────────────────────────┤
│ FirebaseAuthValidator::extractToken() ← Bearer token    │
│ FirebaseAuthValidator::verifyToken() ← Valida com FB   │
│ AuthContextLoader::loadFullContext() ← Busca role local │
└─────────────────────────────────────────────────────────┘

Mesma AuthContext, mesmas Permissions
Apenas validação de token muda!
```

## Performance

```
┌─────────────────────────────────────────────────────────┐
│ Request entra                                           │
└────────────┬────────────────────────────────────────────┘
			 │
			 ▼
	┌────────────────────┐
	│ Extract User ID    │ ← O(1) - String em header
	└────────────┬───────┘
				 │
				 ▼
	┌────────────────────────────────────┐
	│ Query DB por ID                    │ ← O(1) - Índice PRIMARY KEY
	└────────────┬───────────────────────┘
				 │
				 ▼
	┌────────────────────────────────────┐
	│ Calcular Permissões                │ ← O(1) - Switch statement
	│ getDefaultPermissions(role)        │         Local, sem DB
	└────────────┬───────────────────────┘
				 │
				 ▼
	┌────────────────────────────────────┐
	│ Handler executa lógica             │
	│ (Pode fazer suas queries)          │
	└────────────┬───────────────────────┘
				 │
				 ▼
	┌────────────────────────────────────┐
	│ Response retorna ao cliente        │ ← Total: 1-2 queries DB
	└────────────────────────────────────┘

Nenhuma N+1 query de permissões!
Índices mantêm performance O(1)
```
