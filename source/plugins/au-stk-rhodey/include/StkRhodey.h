#ifndef AU_STK_RHODEY_H
#define AU_STK_RHODEY_H

#include "AudioUnit.h"

class QtVariantProperty;

namespace stk {
    class Rhodey;
}

class StkRhodey : public AudioUnit
{
public:

    StkRhodey(AudioUnitPlugin *pPlugin);
    ~StkRhodey();

    // ISerializable interface
    void serialize(QVariantMap &data, SerializationContext *pContext) const;
    void deserialize(const QVariantMap &data, SerializationContext *pContext);

protected:

    void processStart();
    void processStop();
    void process();
    void reset();

private:

    void createProperties();

    InputPort *m_pInputFreq;
    InputPort *m_pInputVelocity;
    InputPort *m_pInputNoteOn;

    OutputPort *m_pOutput;

    bool m_noteOn;

    QtVariantProperty *m_pPropPluckPosition;
    QtVariantProperty *m_pPropLoopGain;

    stk::Rhodey *m_pRhodey;
};

#endif // AU_STK_RHODEY_H

