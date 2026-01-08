# Copilot instructions — TaskManager (Qt/qmake)

Purpose: give an AI coding agent the minimal, actionable context to become productive in this Qt6 qmake project.

- Language & Communication
  - Communicate with the maintainer in Russian (use Russian for messages, questions, and explanations directed at the developer).
  - Do NOT write code comments in Russian. Prefer English for all in-source comments and docstrings.
  - Only use non-English code comments if explicitly requested by the maintainer or when a specific locale-dependent explanation is necessary.

- Big picture
  - C++ backend (folders: `database/`, `repositories/`, `src/`, `include/`) supplies data and models to the QML frontend.
  - QML UI lives under `qml/` and is packaged via `qml.qrc`. `main.cpp` loads `qrc:/main.qml` and adds `:/qml` as an import path.
  - Data flow example: `TaskModel::updateTask()` (see `include/taskmodel.cpp`) calls `m_reader.requestTaskBrowse()` which ultimately interacts with database code; low-level DB access is via the singleton `DBManager::instance().execute()` defined in `database/dbmanager.cpp`.

- Build / run / debug (practical)
  - Project uses qmake (.pro file `TaskManager.pro`). Typical local build on Windows (Qt dev shell):

    qmake
    mingw32-make

  - Prefer opening the project in Qt Creator for builds and QML debugging. The `build/` folder in the repo shows typical Debug/Release outputs.
  - To debug QML, enable QML debugging in Qt Creator and run the app from the IDE; for C++ use the built-in debugger.

- Project-specific conventions & patterns
  - Header vs source: public interfaces in `include/` (e.g., `include/taskmodel.h`), implementations in `src/` or top-level files listed in the .pro `SOURCES` (note: some `.cpp` are directly under `include/`). Follow existing layout when adding files.
  - Repositories: `repositories/` contains data access layers (look for `TasksRepository.cpp` — currently empty). Prefer repository classes to encapsulate SQL and return domain objects.
  - Models exposed to QML: use `QAbstractListModel`-style models (example: `include/taskmodel.cpp`) and register them via `qmlRegisterType` or expose singletons. Role names in this project use lowercase identifiers: `firstname`, `surname`, `phoneNumber`.
  - Resource imports: `main.cpp` calls `engine.addImportPath(":/qml")`; QML modules live under `qml/` (e.g., `qml/core/ResourceManager.qml`) and are referenced from QML with the bundled resource prefix.

- Integration points & external dependencies
  - Qt modules: `quick`, `quickcontrols2`, `core`, `sql` (see `TaskManager.pro`). Expect `QSqlDatabase` usage and platform SQLite or other driver configuration in `database/dbconnectionmanager.*`.
  - Where to change behavior: UI in `qml/`, domain logic in `repositories/` and `src/`, persistence in `database/`.
  - Watch for C++/QML registrations: `qmlRegisterType<TaskModel>(...)` (search in repo) and engine import path (`main.cpp`).

- Concrete examples to reference when making changes
  - App entry: `main.cpp` — sets `QGuiApplication`, adds QML import path, loads `qrc:/main.qml`.
  - Model example: `include/taskmodel.cpp` — `updateTask()` uses `m_reader.requestTaskBrowse()`; implementers should mirror its interaction pattern when adding models.
  - DB access: `database/dbmanager.cpp` — `DBManager::execute()` returns `std::pair<DBResult, QSqlQuery>`; callers expect this pattern.

- Things the agent should NOT assume
  - `repositories/TasksRepository.cpp` is currently empty — do not assume repository logic exists until implemented.
  - There are no automated tests detected; do not add heavy test assumptions without asking the maintainer for preferred test harness.

- Recommended first steps for edits or feature work
  1. Open `main.cpp` and `TaskManager.pro` to confirm build targets and qmake variables.
 2. Inspect `include/taskmodel.cpp` and `include/taskreader.cpp` to understand how model reads are implemented before changing UI bindings.
 3. If changing SQL, update `database/dbtypes.h` and verify queries via `DBManager::execute()` usage patterns.
 4. When adding QML components, put them under `qml/components/` and add any new files to `qml.qrc` if they must be bundled.

- Questions to ask the maintainer (if unclear)
  - Where is the runtime database (file location / connection string)? Check `database/dbconnectionmanager.cpp` or ask for sample DB.
  - Is `repositories/TasksRepository.cpp` intentionally left unimplemented or omitted by accident?

If anything above is incorrect or you want more examples (traces of SQL, sample QML bindings), tell me which area to expand and I will update this file.
