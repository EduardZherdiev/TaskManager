#ifndef DBTYPES_H
#define DBTYPES_H
#include <QVariantList>

namespace DBTypes
{
enum class DBResult {
    OK,
    FAIL
};

enum class DBTables {
    Users = 1,
    Tasks ,
    Feedbacks
};

enum class DBState {
    OK,
    ERROR_NO_DRIVER,
    ERROR_WORKSPACE,
    ERROR_TABLES,
    ERROR_OPENING
};

enum class TaskState : int {
    Active = 0,
    Completed = 1,
    Archived = 2
};

using DBEntry = QVariantList;
using DBIndex = int;

}
#endif // DBTYPES_H
