#include <QDebug>
#include <QtVariantPropertyManager>
#include <QtVariantProperty>
#include <qmath.h>
#include "Application.h"
#include "ISignalChain.h"
#include "../include/LHPFilter.h"


LHPFilter::LHPFilter(AudioUnitPlugin *pPlugin)
    : AudioUnit(pPlugin),
      m_filter()
{
    m_pInput = addInput("in", Signal::Type_Float);
    m_pInputCutOffFreq = addInput("f", Signal::Type_Float);

    m_pOutput = addOutput("out", Signal::Type_Float);

    createProperties();
}

LHPFilter::~LHPFilter()
{
}

void LHPFilter::serialize(QVariantMap &data, SerializationContext *pContext) const
{
    Q_ASSERT(pContext != nullptr);
    data["filterType"] = m_pFilterType->value();
    AudioUnit::serialize(data, pContext);
}

void LHPFilter::deserialize(const QVariantMap &data, SerializationContext *pContext)
{
    Q_ASSERT(pContext != nullptr);
    m_pFilterType->setValue(data["filterTime"]);
    AudioUnit::deserialize(data, pContext);
}

void LHPFilter::processStart()
{
    m_filter.setSampleRate(signalChain()->sampleRate());
    m_filter.reset();
}

void LHPFilter::processStop()
{
}

void LHPFilter::process()
{
    int type = m_pFilterType->value().toInt();
    m_filter.setType(type == 0 ? VAOnePoleFilter::Type_LP : VAOnePoleFilter::Type_HP);
    m_filter.setCutOffFrequency(m_pInputCutOffFreq->value().asFloat);
    m_pOutput->setFloatValue(m_filter.doFilter(m_pInput->value().asFloat));
}

void LHPFilter::reset()
{
    m_filter.reset();
}

void LHPFilter::createProperties()
{
    QtVariantProperty *pRoot = rootProperty();

    m_pFilterType = propertyManager()->addProperty(QtVariantPropertyManager::enumTypeId(), "Filter type");
    QVariantList list;
    list << "Low-pass" << "High-pass";
    m_pFilterType->setAttribute("enumNames", list);
    m_pFilterType->setValue(0);
    pRoot->addSubProperty(m_pFilterType);

}
