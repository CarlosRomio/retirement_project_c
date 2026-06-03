# Copilot Instructions

## Diretrizes de projeto
- Quando houver erro de registro de componentes do framework, investigar o mecanismo de registro real antes de sugerir arquivos .cpp vazios como correção.
- Modelagem de Trip: tasks deve ser relação de múltiplas tarefas por viagem (N:N), além de manter participantes; adicionar campo de owner (usuário criador/dono) separado dos participantes para permissões.

## Autenticação da API
- Para a autenticação simplificada atual da API, usar apenas o id como identificador nas rotas /me; o email não precisa ser exigido no header por enquanto.