ALTER TABLE tasks
	ADD COLUMN IF NOT EXISTS trip_id TEXT REFERENCES trips(id) ON DELETE CASCADE,
	ADD COLUMN IF NOT EXISTS owner_user_id TEXT REFERENCES users(id) ON DELETE RESTRICT,
	ADD COLUMN IF NOT EXISTS name TEXT,
	ADD COLUMN IF NOT EXISTS description TEXT,
	ADD COLUMN IF NOT EXISTS credits INTEGER,
	ADD COLUMN IF NOT EXISTS completed BOOLEAN DEFAULT FALSE;

CREATE TABLE IF NOT EXISTS task_completed_users (
	task_id TEXT NOT NULL REFERENCES tasks(id) ON DELETE CASCADE,
	user_id TEXT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
	PRIMARY KEY (task_id, user_id)
);

CREATE INDEX IF NOT EXISTS idx_tasks_trip_id ON tasks(trip_id);
CREATE INDEX IF NOT EXISTS idx_tasks_owner_user_id ON tasks(owner_user_id);
CREATE INDEX IF NOT EXISTS idx_task_completed_users_task_id ON task_completed_users(task_id);
CREATE INDEX IF NOT EXISTS idx_task_completed_users_user_id ON task_completed_users(user_id);
