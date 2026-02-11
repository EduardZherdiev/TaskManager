TaskManager (Qt/QML)
===================

A desktop task manager client built with Qt 6 and QML. The app provides user authentication, task tracking, and feedback features, and can synchronize data with a server via a REST-style API.

Overview
--------
- Qt Quick (QML) UI with reusable components and dialogs.
- C++ backend models for users, tasks, and feedback.
- Optional network sync through a `NetworkClient` with JWT access/refresh tokens.
- OpenGL-based bar charts for basic data visualization.

Key Features
------------
- Sign-in and registration flows with optional remembered login.
- Task CRUD: create, update, delete, and list tasks.
- Feedback CRUD: rate and describe feedback items.
- Local data handling plus server synchronization (when configured).
- Light/Dark theme toggle saved in settings.

Tech Stack
----------
- Qt 6 (Qt Quick, Qt Quick Controls 2, Qt SQL, Qt Network, OpenGL)
- C++11
- QML for UI
- Argon2 for password hashing (embedded in the project)

Project Structure
-----------------
- `main.cpp` and `main.qml`: application entry points.
- `src/` and `include/`: C++ models and helpers.
- `qml/`: QML components, dialogs, and pages.
- `network/`: REST client and sync logic.
- `database/`: local database access layer.
- `assets/`: images and icons.

Requirements
------------
- Qt 6.x with modules: Quick, QuickControls2, SQL, Network, OpenGL
- C++ compiler supported by Qt (e.g., MinGW on Windows)
- Optional: TaskManagerServer (see the sibling `TaskManagerServer` folder)

Build (Qt Creator)
------------------
1. Open `TaskManager.pro` in Qt Creator.
2. Select a Qt 6 kit.
3. Configure and build the project.
4. Run the app from Qt Creator.

Build (Command Line)
--------------------
From the `TaskManager` folder:

```bash
qmake TaskManager.pro
make
```

On Windows with MinGW, use the `mingw32-make` tool if `make` is not available.

Server Integration
------------------
The app can connect to a server for authentication and data sync. The client uses a configurable base URL and JWT tokens:

- Base URL is set via `NetworkClient::setBaseUrl`.
- Access/refresh tokens are set via `NetworkClient::setAccessToken` and `NetworkClient::setRefreshToken`.

See the `TaskManagerServer` folder for the Python backend and its setup instructions.

Configuration Notes
-------------------
- The UI applies the saved theme at startup and supports remembered login.
- Auto-login uses a stored password hash (not the raw password).
- If you change server routes or ports, update the base URL accordingly.

Troubleshooting
---------------
- If the app shows empty lists, verify the database connection and/or server health.
- If network calls fail, check the server URL and token values.
- For OpenGL issues, ensure your GPU drivers are up to date.

License
-------
This project includes Argon2, which is distributed under its own license. See `argon2/LICENSE` for details.
