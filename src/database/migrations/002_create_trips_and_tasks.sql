CREATE TABLE IF NOT EXISTS tasks (
	id TEXT PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS trips (
	id TEXT PRIMARY KEY,
	name TEXT NOT NULL,
	created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	task_id TEXT NULL REFERENCES tasks(id) ON DELETE SET NULL
);

CREATE TABLE IF NOT EXISTS trip_users (
	trip_id TEXT NOT NULL REFERENCES trips(id) ON DELETE CASCADE,
	user_id TEXT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
	PRIMARY KEY (trip_id, user_id)
);

CREATE INDEX IF NOT EXISTS idx_trip_users_trip_id ON trip_users(trip_id);
CREATE INDEX IF NOT EXISTS idx_trip_users_user_id ON trip_users(user_id);
