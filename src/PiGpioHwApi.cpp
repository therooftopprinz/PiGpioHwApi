#include <HwApi.hpp>
#include <exception>
#include <pigpiod_if2.h>
#include <Logger.hpp>
#include <iomanip>
#include <SX127x.hpp>

namespace hwapi
{

class PiGpio
{
public:
    static std::shared_ptr<ISpi>  createSpi(uint8_t channel);
    static std::shared_ptr<IGpio> createGpio();
    static void setup();
    static void teardown();
    static int getPiGpioHandle() {return mPiGpioHandle;}
    static int mPiGpioHandle = -1;
};

class Spi : public ISpi
{
public:
    Spi(uint8_t pChannel)
    {
        mHandle  = spi_open(PiGpio::getPiGpioHandle(), pChannel, 921600,
            //bbbbbbRTnnnnWAuuupppmm
            0b0000000000000000000000);
        if (mHandle<0)
        {
            throw std::runtime_error(std::string{} + "spi_open failed! handle=" + std::to_string(mHandle));
        }
    }

    ~Spi()
    {
        if (spi_close>=0)
        {
            spi_close(PiGpio::getPiGpioHandle(), mHandle);
        }
    }

    int read(uint8_t *data, unsigned count) override
    {
        throw std::runtime_error("spi read/write not implemented!");
    }

    int write(uint8_t *data, unsigned count) override
    {
        throw std::runtime_error("spi read/write not implemented!");
    }

    int xfer(uint8_t *dataOut, uint8_t *dataIn, unsigned count) override
    {
        return spi_xfer(PiGpio::getPiGpioHandle(), mHandle, (char*)dataOut, (char*)dataIn, count);
    }
};

class Gpio : public IGpio
{
public:
    int get(unsigned pGpio) override
    {
        return gpio_read(PiGpio::getPiGpioHandle(), pGpio);
    }

    int set(unsigned pGpio, unsigned pLevel) override
    {
        return gpio_write(PiGpio::getPiGpioHandle(), pGpio, pLevel);
    }

    int registerCallback(unsigned pUserGpio, Edge pEdge, std::function<void(uint32_t tick)> pCb) override
    {
        unsigned edge = 0;
        if (Edge::RISING == pEdge)
        {
            edge = RISING_EDGE;
        }
        else
        {
            edge = FALLING_EDGE;
        }

        fnCb[pUserGpio] = pCb;
        return callback(PiGpio::getPiGpioHandle(), pUserGpio, edge, &Gpio::cb);
    }
    int deregisterCallback(int pCallbackId) override
    {
        return callback_cancel(pCallbackId);
    }

private:
    static void cb(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
    {
        fnCb[user_gpio](tick);
    }
    static std::array<std::function<void(uint32_t tick)>, 20> fnCb;
};

std::shared_ptr<ISpi> PiGpio::createSpi(uint8_t channel)
{
    return std::make_shared<Spi>(channel);
}

std::shared_ptr<IGpio> PiGpio::createGpio()
{
    return std::make_shared<Gpio>();
}

void PiGpio::setup()
{
    char port[] = {'8','8','8','8',0};
    mPiGpioHandle = pigpio_start(nullptr, port);
    if (mPiGpioHandle<0)
    {
        throw std::runtime_error(std::string{} + "connecting to pigpiod failed! handle=" + std::to_string(mPiGpioHandle));
    }
}

void PiGpio::teardown()
{
    if (mPiGpioHandle>=0)
    {
        pigpio_stop(mPiGpioHandle);
    }
}

void setup()
{
    PiGpio::setup();
}

void teardown()
{
    PiGpio::teardown();
}

std::shared_ptr<ISpi>  getSpi(uint8_t pChannel)
{
    return PiGpio::createSpi(pChannel)
}

std::shared_ptr<IGpio> getGpio()
{
    return PiGpio::createGpio();
}

}