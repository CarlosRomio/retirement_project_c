-- Create admin seed user
INSERT INTO users (id, name, email, role, created_at)
VALUES ('admin-seed-001', 'Administrator', 'admin@reiment.local', 'ADMIN', CURRENT_TIMESTAMP)
ON CONFLICT DO NOTHING;
