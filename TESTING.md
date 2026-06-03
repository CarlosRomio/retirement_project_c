# 🧪 Guia de Testes - Sistema de Autorização

## Pré-requisitos

- Servidor rodando em `http://localhost:8080`
- Base de dados PostgreSQL configurada
- Cliente HTTP (curl, Postman, ou similar)

## Admin Seed

Ao iniciar a aplicação, um usuário admin é criado automaticamente:

```
ID: admin-seed-001
Email: admin@reiment.local
Name: Administrator
Role: ADMIN
```

Use o `user_id` **admin-seed-001** no header para requisições como admin.

## Rotas Públicas

### 1. Criar Novo Usuário (Signup)

**Request:**
```bash
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{
	"name": "João Silva",
	"email": "joao@example.com"
  }'
```

**Response (201):**
```json
{
  "id": "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
}
```

### 2. Login

**Request:**
```bash
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{
	"email": "joao@example.com",
	"password": "qualquer_senha"
  }'
```

**Response (200):**
```json
{
  "user_id": "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6",
  "name": "João Silva",
  "email": "joao@example.com",
  "role": "USER"
}
```

## Rotas Autenticadas - User

### 1. Ver Perfil Próprio

**Request:**
```bash
curl -X GET http://localhost:8080/api/users/me \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
```

**Response (200):**
```json
{
  "id": "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6",
  "name": "João Silva",
  "email": "joao@example.com",
  "role": "USER"
}
```

**Erro (401 - sem autenticação):**
```bash
curl -X GET http://localhost:8080/api/users/me
```

Response:
```json
{
  "error": "Authentication failed"
}
```

### 2. Atualizar Perfil

**Request:**
```bash
curl -X PUT http://localhost:8080/api/users/me \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6" \
  -H "Content-Type: application/json" \
  -d '{
	"name": "João Silva Updated"
  }'
```

**Response (200):**
```json
{
  "id": "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6",
  "name": "João Silva Updated",
  "email": "joao@example.com",
  "role": "USER"
}
```

## Rotas Autenticadas - Trips

### 1. Criar Viagem (Autenticado)

**Request:**
```bash
curl -X POST http://localhost:8080/api/trips \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6" \
  -H "Content-Type: application/json" \
  -d '{
	"name": "Viagem para Marte"
  }'
```

**Response (201):**
```json
{
  "id": "trip-123",
  "created_at": "2024-01-15T10:30:00Z"
}
```

**Erro (401 - sem autenticação):**
```json
{
  "error": "Authentication failed"
}
```

### 2. Ver Viagem

**Request:**
```bash
curl -X GET http://localhost:8080/api/trips/trip-123 \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
```

**Response (200):**
```json
{
  "id": "trip-123",
  "name": "Viagem para Marte",
  "created_at": "2024-01-15T10:30:00Z",
  "owner_user_id": "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6",
  "task_ids": [],
  "user_ids": []
}
```

### 3. Deletar Viagem (Owner Only)

**Request (owner):**
```bash
curl -X DELETE http://localhost:8080/api/trips/trip-123 \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
```

**Response (204 No Content):**
```
```

**Erro (403 - não é owner):**
```bash
curl -X DELETE http://localhost:8080/api/trips/trip-123 \
  -H "x-user-id: outro-user-id"
```

Response:
```json
{
  "error": "You don't have permission to modify this resource"
}
```

### 4. Deletar com Admin

**Admin pode deletar qualquer viagem:**
```bash
curl -X DELETE http://localhost:8080/api/trips/trip-123 \
  -H "x-user-id: admin-seed-001"
```

**Response (204 No Content):**
```
```

## Rotas Autenticadas - Tasks

### 1. Criar Tarefa

**Request:**
```bash
curl -X POST http://localhost:8080/api/tasks \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6" \
  -H "Content-Type: application/json" \
  -d '{
	"trip_id": "trip-123",
	"name": "Preparar foguete",
	"description": "Checar combustível",
	"credits": 100
  }'
```

**Response (201):**
```json
{
  "id": "task-456"
}
```

### 2. Ver Tarefa

**Request:**
```bash
curl -X GET http://localhost:8080/api/tasks/task-456 \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
```

**Response (200):**
```json
{
  "id": "task-456",
  "trip_id": "trip-123",
  "owner_user_id": "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6",
  "name": "Preparar foguete",
  "description": "Checar combustível",
  "credits": 100,
  "completed": false,
  "completed_user_ids": []
}
```

### 3. Deletar Tarefa (Owner Only)

**Request (owner):**
```bash
curl -X DELETE http://localhost:8080/api/tasks/task-456 \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
```

**Response (204 No Content):**
```
```

## Casos de Erro

### 1. Sem Autenticação

**Request:**
```bash
curl -X GET http://localhost:8080/api/users/me
```

**Response (401):**
```json
{
  "error": "Authentication failed"
}
```

### 2. Sem Permissão

**Usuário normal tentando criar viagem (USER role):**
```bash
# USER tem permissão CREATE_TRIP, então isso funciona
curl -X POST http://localhost:8080/api/trips \
  -H "x-user-id: user-id" \
  -H "Content-Type: application/json" \
  -d '{"name": "Viagem"}'
```

**Deletar viagem de outro usuário (403):**
```bash
curl -X DELETE http://localhost:8080/api/trips/trip-owner-other \
  -H "x-user-id: user-id"
```

Response:
```json
{
  "error": "You don't have permission to modify this resource"
}
```

### 3. Recurso Não Encontrado

**Request:**
```bash
curl -X GET http://localhost:8080/api/trips/trip-nao-existe \
  -H "x-user-id: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
```

**Response (404):**
```json
{
  "error": "Trip not found"
}
```

## Matriz de Permissões

| Ação | USER | OWNER | ADMIN |
|------|------|-------|-------|
| CREATE_TRIP | ✅ | ✅ | ✅ |
| READ_TRIP | ✅ | ✅ | ✅ |
| UPDATE_TRIP | ❌ | ✅ | ✅ |
| DELETE_TRIP | ❌* | ✅ | ✅ |
| CREATE_TASK | ✅ | ✅ | ✅ |
| READ_TASK | ✅ | ✅ | ✅ |
| UPDATE_TASK | ❌ | ✅ | ✅ |
| DELETE_TASK | ❌* | ✅ | ✅ |
| DELETE_OWN_USER | ✅ | ✅ | ✅ |
| MANAGE_USERS | ❌ | ❌ | ✅ |

\* = Apenas se for o owner

## Postman Collection

```json
{
  "info": {
	"name": "Reiment API - Authorization",
	"schema": "https://schema.getpostman.com/json/collection/v2.1.0/collection.json"
  },
  "item": [
	{
	  "name": "Auth",
	  "item": [
		{
		  "name": "Create User",
		  "request": {
			"method": "POST",
			"url": "{{base_url}}/api/users",
			"body": {
			  "mode": "raw",
			  "raw": "{\"name\": \"João\", \"email\": \"joao@example.com\"}"
			}
		  }
		},
		{
		  "name": "Login",
		  "request": {
			"method": "POST",
			"url": "{{base_url}}/api/auth/login",
			"body": {
			  "mode": "raw",
			  "raw": "{\"email\": \"joao@example.com\", \"password\": \"any\"}"
			}
		  }
		}
	  ]
	}
  ]
}
```

## Próximos Testes

1. ✅ Criar usuário (público)
2. ✅ Login (público)
3. ✅ Ver perfil (autenticado)
4. ✅ Criar viagem (autenticado + permissão)
5. ✅ Deletar viagem própria (owner)
6. ✅ Tentar deletar viagem de outro (403 Forbidden)
7. ✅ Admin deletar qualquer viagem
8. ✅ Criar tarefa (autenticado + permissão)
9. ✅ Deletar tarefa própria (owner)
10. ✅ Tentar acessar sem header (401 Unauthorized)
