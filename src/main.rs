use std::{collections::HashMap, fs, path::Path, sync::RwLock, time::Instant};

use actix_web::{App, HttpResponse, HttpServer, Responder, delete, get, post, put, web};
use serde::{Deserialize, Serialize};
use serde_json::Value;
use uuid::Uuid;

const DB_FILE: &str = "tangue.db.ron";

fn ron_response<T: Serialize>(data: &T) -> HttpResponse {
    match ron::to_string(data) {
        Ok(body) => HttpResponse::Ok()
            .content_type("application/ron")
            .body(body),
        Err(_) => HttpResponse::InternalServerError().body("Serialization error"),
    }
}

#[derive(Clone, Serialize, Deserialize, Debug)]
enum ColumnType {
    String,
    Integer,
    Float,
    Boolean,
    Object,
}

#[derive(Clone, Serialize, Deserialize, Debug)]
struct Column {
    name: String,
    column_type: ColumnType,
}

#[derive(Clone, Serialize, Deserialize, Debug)]
struct Table {
    id: String,
    name: String,
    columns: Vec<Column>,
    rows: Vec<HashMap<String, Value>>,
}

impl Table {
    fn new(name: String, columns: Vec<Column>) -> Self {
        Self {
            id: Uuid::new_v4().to_string(),
            name,
            columns,
            rows: Vec::new(),
        }
    }

    fn validate_row(&self, row: &HashMap<String, Value>) -> Result<(), String> {
        for column in &self.columns {
            match row.get(&column.name) {
                Some(val) => match (&column.column_type, val) {
                    (ColumnType::String, Value::String(_)) => {}
                    (ColumnType::Integer, Value::Number(n)) if n.is_i64() => {}
                    (ColumnType::Float, Value::Number(_)) => {}
                    (ColumnType::Boolean, Value::Bool(_)) => {}
                    (ColumnType::Object, Value::Object(_)) => {}
                    _ => return Err(format!("Column '{}' type mismatch", column.name)),
                },
                None => return Err(format!("Missing required column '{}'", column.name)),
            }
        }
        Ok(())
    }
}

struct Database {
    tables: RwLock<HashMap<String, Table>>,
    started_at: Instant,
}

impl Database {
    fn new() -> Self {
        let database = Self {
            tables: RwLock::new(HashMap::new()),
            started_at: Instant::now(),
        };
        database.load_from_file();
        database
    }

    fn load_from_file(&self) {
        if Path::new(DB_FILE).exists() {
            if let Ok(content) = fs::read_to_string(DB_FILE) {
                if let Ok(tables) = ron::from_str::<HashMap<String, Table>>(&content) {
                    if let Ok(mut store) = self.tables.write() {
                        *store = tables;
                    }
                }
            }
        }
    }

    fn save_to_file(&self) -> Result<(), String> {
        if let Ok(store) = self.tables.read() {
            let content =
                ron::to_string(&*store).map_err(|e| format!("Serialization error: {}", e))?;
            fs::write(DB_FILE, content).map_err(|e| format!("Write error: {}", e))?;
            Ok(())
        } else {
            Err("Database lock is poisoned".to_string())
        }
    }
}

#[derive(Serialize)]
struct HealthResponse {
    status: &'static str,
}

#[derive(Serialize)]
struct StatsResponse {
    tables: usize,
    uptime_ms: u128,
}

#[derive(Serialize)]
struct TableListResponse {
    tables: Vec<String>,
}

#[derive(Serialize)]
struct TableResponse {
    id: String,
    name: String,
    columns: Vec<Column>,
    rows: usize,
}

#[derive(Serialize)]
struct ErrorResponse {
    error: String,
}

#[derive(Deserialize)]
struct CreateTableRequest {
    name: String,
    columns: Vec<Column>,
}

#[derive(Deserialize)]
struct InsertRowRequest {
    row: HashMap<String, Value>,
}

#[get("/")]
async fn root() -> impl Responder {
    ron_response(&HealthResponse { status: "ok" })
}

#[get("/health")]
async fn health() -> impl Responder {
    ron_response(&HealthResponse { status: "ok" })
}

#[get("/statistics")]
async fn statistics(database: web::Data<Database>) -> impl Responder {
    let tables = database.tables.read().map(|m| m.len()).unwrap_or(0);
    let uptime_ms = database.started_at.elapsed().as_millis();

    ron_response(&StatsResponse { tables, uptime_ms })
}

#[get("/tables")]
async fn list_tables(database: web::Data<Database>) -> impl Responder {
    match database.tables.read() {
        Ok(store) => {
            let tables = store.keys().cloned().collect();
            ron_response(&TableListResponse { tables })
        }
        Err(_) => ron_response(&ErrorResponse {
            error: "database lock is poisoned".to_string(),
        }),
    }
}

#[post("/tables")]
async fn create_table(
    payload: web::Json<CreateTableRequest>,
    database: web::Data<Database>,
) -> impl Responder {
    match database.tables.write() {
        Ok(mut store) => {
            if store.contains_key(&payload.name) {
                return HttpResponse::Conflict()
                    .content_type("application/ron")
                    .body(
                        ron::to_string(&ErrorResponse {
                            error: format!("Table '{}' already exists", payload.name),
                        })
                        .unwrap_or_default(),
                    );
            }

            let table = Table::new(payload.name.clone(), payload.columns.clone());
            let response = TableResponse {
                id: table.id.clone(),
                name: table.name.clone(),
                columns: table.columns.clone(),
                rows: 0,
            };

            store.insert(payload.name.clone(), table);

            if let Err(e) = database.save_to_file() {
                return HttpResponse::InternalServerError()
                    .content_type("application/ron")
                    .body(ron::to_string(&ErrorResponse { error: e }).unwrap_or_default());
            }

            HttpResponse::Created()
                .content_type("application/ron")
                .body(ron::to_string(&response).unwrap_or_default())
        }
        Err(_) => HttpResponse::InternalServerError()
            .content_type("application/ron")
            .body(
                ron::to_string(&ErrorResponse {
                    error: "database lock is poisoned".to_string(),
                })
                .unwrap_or_default(),
            ),
    }
}

#[get("/tables/{table_name}")]
async fn get_table(table_name: web::Path<String>, database: web::Data<Database>) -> impl Responder {
    match database.tables.read() {
        Ok(store) => match store.get(table_name.as_str()) {
            Some(table) => ron_response(&TableResponse {
                id: table.id.clone(),
                name: table.name.clone(),
                columns: table.columns.clone(),
                rows: table.rows.len(),
            }),
            None => ron_response(&ErrorResponse {
                error: format!("Table '{}' not found", table_name),
            }),
        },
        Err(_) => ron_response(&ErrorResponse {
            error: "database lock is poisoned".to_string(),
        }),
    }
}

#[post("/tables/{table_name}/rows")]
async fn insert_row(
    table_name: web::Path<String>,
    payload: web::Json<InsertRowRequest>,
    database: web::Data<Database>,
) -> impl Responder {
    match database.tables.write() {
        Ok(mut store) => match store.get_mut(table_name.as_str()) {
            Some(table) => {
                if let Err(e) = table.validate_row(&payload.row) {
                    return ron_response(&ErrorResponse { error: e });
                }

                table.rows.push(payload.row.clone());

                if let Err(e) = database.save_to_file() {
                    return ron_response(&ErrorResponse { error: e });
                }

                #[derive(Serialize)]
                struct RowInsertResponse {
                    table: String,
                    row_inserted: bool,
                    rows_count: usize,
                }

                ron_response(&RowInsertResponse {
                    table: table_name.to_string(),
                    row_inserted: true,
                    rows_count: table.rows.len(),
                })
            }
            None => ron_response(&ErrorResponse {
                error: format!("Table '{}' not found", table_name),
            }),
        },
        Err(_) => ron_response(&ErrorResponse {
            error: "database lock is poisoned".to_string(),
        }),
    }
}

#[get("/tables/{table_name}/rows")]
async fn list_rows(table_name: web::Path<String>, database: web::Data<Database>) -> impl Responder {
    match database.tables.read() {
        Ok(store) => match store.get(table_name.as_str()) {
            Some(table) => {
                #[derive(Serialize)]
                struct RowsResponse {
                    table: String,
                    rows: Vec<HashMap<String, Value>>,
                    count: usize,
                }
                ron_response(&RowsResponse {
                    table: table_name.to_string(),
                    rows: table.rows.clone(),
                    count: table.rows.len(),
                })
            }
            None => ron_response(&ErrorResponse {
                error: format!("Table '{}' not found", table_name),
            }),
        },
        Err(_) => ron_response(&ErrorResponse {
            error: "database lock is poisoned".to_string(),
        }),
    }
}

#[delete("/tables/{table_name}")]
async fn delete_table(
    table_name: web::Path<String>,
    database: web::Data<Database>,
) -> impl Responder {
    match database.tables.write() {
        Ok(mut store) => {
            if store.remove(table_name.as_str()).is_some() {
                if let Err(e) = database.save_to_file() {
                    return ron_response(&ErrorResponse { error: e });
                }
                #[derive(Serialize)]
                struct DeleteResponse {
                    deleted: bool,
                    table: String,
                }
                ron_response(&DeleteResponse {
                    deleted: true,
                    table: table_name.to_string(),
                })
            } else {
                ron_response(&ErrorResponse {
                    error: format!("Table '{}' not found", table_name),
                })
            }
        }
        Err(_) => ron_response(&ErrorResponse {
            error: "database lock is poisoned".to_string(),
        }),
    }
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    let database = web::Data::new(Database::new());

    HttpServer::new(move || {
        App::new()
            .app_data(database.clone())
            .service(root)
            .service(health)
            .service(statistics)
            .service(list_tables)
            .service(create_table)
            .service(get_table)
            .service(insert_row)
            .service(list_rows)
            .service(delete_table)
    })
    .bind(("127.0.0.1", 8080))?
    .run()
    .await
}
