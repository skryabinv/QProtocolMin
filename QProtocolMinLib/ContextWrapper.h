#pragma once

#include <QObject>
#include <memory>

struct min_context;

// CALLBACKS
extern "C" {

void min_tx_byte(uint8_t port, uint8_t byte);
void min_tx_start(uint8_t port);
void min_tx_finished(uint8_t port);
uint16_t min_tx_space(uint8_t port);
void min_application_handler(uint8_t min_id,
                             uint8_t const *min_payload,
                             uint8_t len_payload,
                             uint8_t port);
}

namespace min {

class ContextWrapper : public QObject {
    Q_OBJECT
signals:
    void frameReceived(quint8 frameId, const QByteArray& payload);
    void frameSended(const QByteArray& frameBytes);
public:
    explicit ContextWrapper(QObject* parent = nullptr);
    ~ContextWrapper() override;
    bool isValid() const;
    void send(quint8 frameId, const QByteArray& payload);
    void poll(const QByteArray& bytes);
private:    
    friend void ::min_tx_byte(uint8_t port, uint8_t byte);
    friend void ::min_tx_start(uint8_t port);
    friend void ::min_tx_finished(uint8_t port);
    friend void ::min_application_handler(uint8_t min_id,
                                          uint8_t const *min_payload,
                                          uint8_t len_payload,
                                          uint8_t port);
    void sendFrame();
    void handleFrame(uint8_t frameId,
                     uint8_t const *min_payload,
                     uint8_t len_payload);
    QByteArray mBytesToSend;
    std::unique_ptr<min_context> mContext{};
    quint8 mPortId{};
};

}
