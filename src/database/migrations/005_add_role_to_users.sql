-- Add role column to users table
ALTER TABLE users
	ADD COLUMN IF NOT EXISTS role VARCHAR(50) DEFAULT 'USER' NOT NULL,
	ADD COLUMN IF NOT EXISTS password_hash VARCHAR(255);

-- Create index for role
CREATE INDEX IF NOT EXISTS idx_users_role ON users(role);
