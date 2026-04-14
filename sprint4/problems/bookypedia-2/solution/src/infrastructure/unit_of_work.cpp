#include "unit_of_work.hpp"

namespace infrastructure {

UnitOfWork::UnitOfWork(pqxx::connection& conn)
    : transaction_(conn)
    , author_repo_(transaction_)
    , book_repo_(transaction_)
    , tag_repo_(transaction_) {}

void UnitOfWork::Commit() {
    transaction_.commit();
}

void UnitOfWork::Rollback() {
    transaction_.abort();
}

UnitOfWorkFactory::UnitOfWorkFactory(std::shared_ptr<pqxx::connection> conn)
    : conn_(std::move(conn)) {}

std::unique_ptr<UnitOfWork> UnitOfWorkFactory::Create() {
    return std::make_unique<UnitOfWork>(*conn_);
}

} // namespace infrastructure
