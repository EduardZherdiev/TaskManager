# TaskManager (Qt/qmake)

Minimal Qt Quick app with QML UI and C++ models (Tasks/Users/Callbacks) backed by SQLite.

## Build (Qt 6, qmake + MinGW on Windows)
1. Open Qt command prompt.
2. In repo root:
   ```
   qmake
   mingw32-make
   ```
3. Run the produced executable from `build/.../debug/` or `release/`.

## Project layout
- `main.cpp` — app entry, registers models, loads `qrc:/main.qml`.
- `qml/` — QML UI components and screens, packaged via `qml.qrc`.
- `database/` — SQLite connection/queries.
- `include/` + `src/` — domain classes and models exposed to QML.
- `repositories/` — data access layer stubs.

## Notes
- SQLite DB is created next to the executable as `TaskManagerDB.sqlite`.
- Test data insertion is controlled in `database/dbprocessing.cpp`.
- QML imports add `:/qml` resource path via `engine.addImportPath` in `main.cpp`.
