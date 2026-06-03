ALTER TABLE trips
	ADD COLUMN IF NOT EXISTS owner_user_id TEXT REFERENCES users(id) ON DELETE RESTRICT;

CREATE TABLE IF NOT EXISTS trip_tasks (
	trip_id TEXT NOT NULL REFERENCES trips(id) ON DELETE CASCADE,
	task_id TEXT NOT NULL REFERENCES tasks(id) ON DELETE CASCADE,
	PRIMARY KEY (trip_id, task_id)
);

CREATE INDEX IF NOT EXISTS idx_trip_tasks_trip_id ON trip_tasks(trip_id);
CREATE INDEX IF NOT EXISTS idx_trip_tasks_task_id ON trip_tasks(task_id);
