#include "ContextWrapper.h"
#include "min/target/min.h"
#include <limits>

constexpr auto sMaxPortsCount = 10;
static std::array<min::ContextWrapper*, sMaxPortsCount> sContextStorage{};

void min_tx_byte(uint8_t port, uint8_t byte) {
    assert(sContextStorage[port] != nullptr);
    sContextStorage[port]->mBytesToSend.append(byte);
}

void min_tx_start(uint8_t port) {
    assert(sContextStorage[port] != nullptr);
    sContextStorage[port]->mBytesToSend.clear();
}

void min_tx_finished(uint8_t port) {
    assert(sContextStorage[port] != nullptr);
    sContextStorage[port]->sendFrame();
}

uint16_t min_tx_space(uint8_t port) {
    assert(sContextStorage[port] != nullptr);
    return std::numeric_limits<uint16_t>::max();
}

void min_application_handler(uint8_t min_id,
                             uint8_t const *min_payload,
                             uint8_t len_payload,
                             uint8_t port) {
    assert(sContextStorage[port] != nullptr);
    sContextStorage[port]->handleFrame(min_id, min_payload, len_payload);
}


namespace min {

static auto findEmptyContextId() -> std::optional<quint8> {
    static_assert(sContextStorage.size() < std::numeric_limits<quint8>::max());
    for(quint8 id = 0; id < sContextStorage.size(); id++) {
        if(sContextStorage[id] == nullptr) {
            return id;
        }
    }
    return std::nullopt;
}

ContextWrapper::ContextWrapper(QObject* parent)
    : QObject{parent} {
    auto id = findEmptyContextId();
    if(!id.has_value()) {
        throw std::runtime_error("No port available");
    }
    mPortId = *id;
    sContextStorage[mPortId] = this;
    mContext = std::make_unique<min_context>();
    min_init_context(mContext.get(), mPortId);
}

ContextWrapper::~ContextWrapper() {
    sContextStorage[mPortId] = nullptr;
}

void ContextWrapper::send(quint8 id, const QByteArray& payload) {
    assert(mContext != nullptr);
    min_send_frame(mContext.get(), mPortId,
                   reinterpret_cast<const uint8_t*>(payload.data()),
                   payload.size());
}

void ContextWrapper::poll(const QByteArray& bytes) {
    assert(mContext != nullptr);
    min_poll(mContext.get(),
             reinterpret_cast<const uint8_t*>(bytes.data()),
             bytes.size());
}

void ContextWrapper::sendFrame() {
    emit frameSended(mBytesToSend);
}

void ContextWrapper::handleFrame(uint8_t min_id,
                                 const uint8_t* min_payload,
                                 uint8_t len_payload) {
    auto payload = QByteArray{reinterpret_cast<const char*>(min_payload), len_payload};
    emit frameReceived(min_id, payload);
}

}
