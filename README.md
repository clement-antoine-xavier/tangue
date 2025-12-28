# Tangue

**A minimalist, high-performance database exposing a clean HTTP REST API for effortless data management. Built for speed, simplicity, and scalability, Tangue focuses on low-level efficiency and developer-friendly design—perfect for modern applications, local deployments, and educational use.**

## Features

- **Lightweight and Fast**: Optimized for performance with minimal resource usage.

- **RESTful API**: Easy-to-use HTTP interface for seamless integration with web applications.

- **RON Serialization**: Uses Rusty Object Notation for human-readable, Rust-idiomatic data exchange.

- **Typed Tables**: Define schemas with type constraints (String, Integer, Float, Boolean, Object).

- **File Persistence**: Automatic saving to `tangue.db.ron` with recovery on startup.

- **Scalable**: Designed to handle growing data needs with ease.

- **Simple Setup**: Quick installation and configuration for immediate use.

- **Cross-Platform**: Runs on various operating systems including Windows, macOS, and Linux.

- **Open Source**: Free to use and modify under the MIT License.

- **Educational Focus**: Ideal for learning database concepts and web development.

## Quickstart

1) Start the server:

```bash
cargo run
```

1) Create a table with schema (RON format):

```bash
curl -X POST \
  -H "Content-Type: application/ron" \
  -d '(name:"users",columns:[(name:"id",col_type:Integer),(name:"name",col_type:String),(name:"active",col_type:Boolean)])' \
  http://127.0.0.1:8080/tables
```

1) Insert rows into the table (RON format):

```bash
curl -X POST \
  -H "Content-Type: application/ron" \
  -d '(row:(id:1,name:"Alice",active:true))' \
  http://127.0.0.1:8080/tables/users/rows
```

1) Query table data:

```bash
curl http://127.0.0.1:8080/tables/users/rows
```

1) List all tables:

```bash
curl http://127.0.0.1:8080/tables
```

1) Delete a table:

```bash
curl -X DELETE http://127.0.0.1:8080/tables/users
```

## How It Works

Tangue now operates as a **typed table database** with **file persistence** using **RON serialization**:

1. **RON Serialization**: All requests and responses use RON (Rusty Object Notation), a human-readable Rust-based format. It's more compact than JSON while maintaining clarity.

2. **Typed Tables**: Define tables with schema (columns and types) in real-time. Supported types: `String`, `Integer`, `Float`, `Boolean`, `Object`.

3. **Type Validation**: Each row inserted is validated against the table schema. Type mismatches are rejected with clear errors.

4. **In-Memory Storage**: Tables are stored in a concurrent `HashMap<String, Table>` protected by `RwLock` for thread-safe access.

5. **File Persistence**: All data is automatically saved to `tangue.db.ron` in RON format on every write operation. On startup, the database automatically loads from this file.

6. **Request Handling**: The server binds to `127.0.0.1:8080` and routes HTTP requests using Actix-web.

7. **State Sharing**: The `Database` struct is wrapped in `web::Data` and shared across worker threads.

## API

### Health & Stats

- `GET /` → `(status:"ok")`
- `GET /health` → `(status:"ok")`
- `GET /statistics` → `(tables:<count>,uptime_ms:<elapsed>)`

### Table Management

- `GET /tables` → `(tables:["table1","table2",...])`
- `POST /tables` with RON schema → Create a new table with typed columns
  - Request: `(name:"users",columns:[(name:"id",col_type:Integer),...])`
  - Response: `(id:"uuid",name:"users",columns:[...],rows:0)`
- `GET /tables/{table_name}` → Get table info (columns, row count)
- `DELETE /tables/{table_name}` → Delete table

### Row Operations

- `POST /tables/{table_name}/rows` with row data → Insert row (validates against schema)
  - Request: `(row:(id:1,name:"Alice",...))`
  - Response: `(table:"users",row_inserted:true,rows_count:1)`
- `GET /tables/{table_name}/rows` → List all rows in table

## Notes

- Data persists in `tangue.db.ron` and is automatically loaded on startup.
- RON format is more compact and Rust-idiomatic than JSON.
- Type validation ensures data consistency before insertion.
- Keep payloads small—this is a lightweight database designed for local development and experimentation.
