# ⚡ Referência Rápida - Comandos e Snippets

## 🔧 Compilação

```bash
# Build em debug
cd C:\Users\Carlos\source\repos\retirement_project_c-
cmake --build build --config Debug

# Build em release
cmake --build build --config Release

# Clean e rebuild
rm -r build
cmake -B build
cmake --build build --config Debug
```

## 🏃 Executar

```bash
# Direct run
./reiment

# Com logs
./reiment 2>&1 | tee output.log

# Background
./reiment &

# Stop running instance
pkill reiment
```

## 🧪 Testes Rápidos

### Criar usuário
```bash
curl -X POST http://localhost:8080/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"Test User","email":"test@example.com"}'
```

### Login
```bash
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email":"test@example.com","password":"test"}'
```

### Obter perfil
```bash
curl -X GET http://localhost:8080/api/users/me \
  -H "x-user-id:USER_ID_HERE"
```

### Admin login
```bash
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email":"admin@reiment.local","password":"admin"}'
```

## 🗄️ Banco de Dados

### Conectar ao PostgreSQL
```bash
psql -U postgres -d travelapp -h 172.20.0.1
```

### Queries úteis
```sql
-- Ver todos os usuários
SELECT id, name, email, role, created_at FROM users ORDER BY created_at DESC;

-- Ver admin seed
SELECT * FROM users WHERE id = 'admin-seed-001';

-- Ver todas as viagens
SELECT id, name, owner_user_id, created_at FROM trips;

-- Ver todas as tarefas
SELECT id, trip_id, owner_user_id, name FROM tasks;

-- Deletar usuário de teste
DELETE FROM users WHERE email = 'test@example.com';

-- Resetar admin seed
DELETE FROM users WHERE id = 'admin-seed-001';
-- Será recriado na próxima execução
```

## 📝 Adicionar Nova Rota

### Passo 1: Criar handler no controller
```cpp
void MyController::newHandler(
	const drogon::HttpRequestPtr& req,
	std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
	AUTH_REQUIRE_AUTHENTICATED(req, callback);
	AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::PERMISSION_NAME, callback);

	// Handler code here
}
```

### Passo 2: Registrar rota
```cpp
// No método das rotas do controller
METHOD_LIST_BEGIN
	ADD_METHOD_TO(MyController::newHandler, "/api/endpoint", drogon::Post);
METHOD_LIST_END
```

### Passo 3: Compilar e testar
```bash
cmake --build build --config Debug
./reiment
curl -X POST http://localhost:8080/api/endpoint \
  -H "x-user-id: admin-seed-001"
```

## 🔒 Adicionar Nova Permissão

### Passo 1: Adicionar enum
```cpp
// em auth-permission.hpp
enum class Permission {
	// ... existing
	MY_NEW_PERMISSION  // ← Add here
};
```

### Passo 2: Adicionar ao getDefaultPermissions()
```cpp
case Role::OWNER:
	// ... existing
	perms.push_back(Permission::MY_NEW_PERMISSION);
	break;
```

### Passo 3: Usar no controller
```cpp
AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::MY_NEW_PERMISSION, callback);
```

## 🐛 Debug

### Adicionar logs
```cpp
#include <iostream>

std::cout << "User ID: " << authCtx->user_id << std::endl;
std::cout << "Role: " << auth::roleToString(authCtx->role) << std::endl;
std::cout << "Permissions count: " << authCtx->permissions.size() << std::endl;

// Check specific permission
if (authCtx->hasPermission(auth::Permission::CREATE_TRIP)) {
	std::cout << "User has CREATE_TRIP permission" << std::endl;
}
```

### Verificar header
```cpp
auto header = req->getHeader("x-user-id");
std::cout << "Header value: " << header << std::endl;
```

## 📊 Postman

### Variáveis de ambiente
```json
{
  "base_url": "http://localhost:8080",
  "user_id": "admin-seed-001",
  "admin_email": "admin@reiment.local",
  "test_email": "test@example.com"
}
```

### Pré-requisitos para requests
```javascript
// Script para salvar user_id da resposta
pm.environment.set("user_id", pm.response.json().user_id);
```

## 🔄 Migrations

### Executar migrations
```bash
# Automático na inicialização
./reiment

# Verificar se criou as tabelas
psql -U postgres -d travelapp -h 172.20.0.1
\dt
```

### Rollback (deletar e recriar)
```sql
-- Drop e recriar tabelas
DROP TABLE IF EXISTS task_completed_users CASCADE;
DROP TABLE IF EXISTS trip_tasks CASCADE;
DROP TABLE IF EXISTS trip_users CASCADE;
DROP TABLE IF EXISTS tasks CASCADE;
DROP TABLE IF EXISTS trips CASCADE;
DROP TABLE IF EXISTS users CASCADE;
-- Executa ./reiment para rodar migrations novamente
```

## 🚀 Deploy Checklist

```bash
# 1. Compilar
cmake --build build --config Release

# 2. Testar rotas públicas
curl -X POST http://localhost:8080/api/users ...

# 3. Testar autenticação
curl -X POST http://localhost:8080/api/auth/login ...

# 4. Testar rotas protegidas
curl -X GET http://localhost:8080/api/users/me \
  -H "x-user-id: admin-seed-001"

# 5. Testar permissões (403 esperado)
curl -X DELETE http://localhost:8080/api/trips/trip-id \
  -H "x-user-id: user-sem-permissao"

# 6. Testar admin (204 esperado)
curl -X DELETE http://localhost:8080/api/trips/trip-id \
  -H "x-user-id: admin-seed-001"

# ✅ Se tudo passou, ready for deploy!
```

## 📋 Estrutura de Uma Requesição Completa

```
┌─ Request
│  ├─ Method: POST
│  ├─ URL: /api/trips
│  ├─ Headers:
│  │  ├─ Content-Type: application/json
│  │  └─ x-user-id: user-123
│  └─ Body:
│     └─ {"name": "My Trip"}
│
├─ Processing
│  ├─ AuthContext loaded
│  ├─ Permissions checked
│  ├─ Business logic
│  └─ Database write
│
└─ Response
   ├─ Status: 201 Created
   └─ Body:
	  └─ {"id": "trip-abc", "created_at": "2024-01-15T10:30:00Z"}
```

## 🆘 Troubleshooting Rápido

| Problema | Solução |
|----------|---------|
| 401 Unauthorized | Adicione header `x-user-id` |
| 403 Forbidden | User não tem permissão ou não é owner |
| 404 Not Found | Recurso não existe |
| 400 Bad Request | Valide JSON e campos obrigatórios |
| Connection refused | Banco PostgreSQL não está rodando |
| Compilation error | Verifique includes em auth-helpers.hpp |

## 📚 Referência de Macros

```cpp
// Require authentication (carrega authCtx)
AUTH_REQUIRE_AUTHENTICATED(req, callback);

// Require specific permission
AUTH_REQUIRE_PERMISSION(authCtx, auth::Permission::CREATE_TRIP, callback);

// Require ownership (ou admin)
AUTH_REQUIRE_OWNERSHIP(authCtx, resource.owner_id, callback);

// Require admin
AUTH_REQUIRE_ADMIN(authCtx, callback);
```

## 🔐 Admin Seed

```
ID:    admin-seed-001
Email: admin@reiment.local
Name:  Administrator
Role:  ADMIN

Use sempre que precisar testar como admin:
curl ... -H "x-user-id: admin-seed-001"
```

## 📱 Teste com cURL Template

```bash
# Salve como script test.sh

#!/bin/bash

BASE_URL="http://localhost:8080"
ADMIN_ID="admin-seed-001"

echo "=== Testing Authorization System ==="

echo -e "\n1. Creating user..."
RESPONSE=$(curl -s -X POST $BASE_URL/api/users \
  -H "Content-Type: application/json" \
  -d '{"name":"Test","email":"test@example.com"}')
USER_ID=$(echo $RESPONSE | jq -r '.id')
echo "Created user: $USER_ID"

echo -e "\n2. Logging in..."
curl -s -X POST $BASE_URL/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"email":"test@example.com","password":"test"}' | jq '.'

echo -e "\n3. Getting user profile..."
curl -s -X GET $BASE_URL/api/users/me \
  -H "x-user-id: $USER_ID" | jq '.'

echo -e "\n4. Creating trip as user..."
TRIP_RESPONSE=$(curl -s -X POST $BASE_URL/api/trips \
  -H "x-user-id: $USER_ID" \
  -H "Content-Type: application/json" \
  -d '{"name":"Test Trip"}')
TRIP_ID=$(echo $TRIP_RESPONSE | jq -r '.id')
echo "Created trip: $TRIP_ID"

echo -e "\n5. Trying to delete as different user (should fail)..."
curl -s -X DELETE $BASE_URL/api/trips/$TRIP_ID \
  -H "x-user-id: other-user-id" | jq '.'

echo -e "\n6. Deleting as admin (should succeed)..."
curl -s -X DELETE $BASE_URL/api/trips/$TRIP_ID \
  -H "x-user-id: $ADMIN_ID" | jq '.'

echo -e "\n=== Tests Complete ==="
```

---

**Dica:** Salve este arquivo como referência rápida enquanto desenvolve!
