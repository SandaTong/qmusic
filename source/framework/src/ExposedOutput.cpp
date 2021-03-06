/*
                          qmusic

    Copyright (C) 2015 Arthur Benilov,
    arthur.benilov@gmail.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.
*/

#include <QtVariantPropertyManager>
#include <QtVariantProperty>
#include <QGraphicsWidget>
#include <qmath.h>
#include "Application.h"
#include "ISignalChain.h"
#include "ExposedOutput.h"

const QColor cItemColor(180, 120, 220);

ExposedOutput::ExposedOutput(AudioUnitPlugin *pPlugin)
    : AudioUnit(pPlugin),
      m_pReferencedOutputPort(nullptr)
{
    m_pInput = addInput();
    createProperties();

    m_pNameItem = nullptr;
}

ExposedOutput::~ExposedOutput()
{
}

void ExposedOutput::fastUpdate()
{
    for (AudioUnit *pAu : m_updateChain) {
        pAu->fastUpdate();
    }
}

void ExposedOutput::processStart()
{
    Q_ASSERT(m_pReferencedOutputPort != nullptr);

    m_updateChain = updateChain();
}

void ExposedOutput::processStop()
{
}

void ExposedOutput::process()
{
    Q_ASSERT(m_pReferencedOutputPort != nullptr);

    float value = m_pReferencedOutputPort->getValue();
    // Mix the input with whatever has been already assigned to referenced port
    m_pInput->update();
    m_pReferencedOutputPort->setValue(value + m_pInput->getValue());
}

void ExposedOutput::reset()
{
}

QGraphicsItem* ExposedOutput::graphicsItem()
{
    if (m_pNameItem == nullptr) {
        m_pNameItem = new QGraphicsSimpleTextItem();
        QBrush brush(QColor(60, 60, 160));
        m_pNameItem->setBrush(brush);

        QFont font = m_pNameItem->font();
        font.setBold(true);
        m_pNameItem->setFont(font);

        m_pNameItem->setText(QString(" %1").arg(m_pPropName->valueText()));
    }
    return m_pNameItem;
}

QColor ExposedOutput::color() const
{
    return cItemColor;
}

int ExposedOutput::flags() const
{
    return Flag_NoTitle | Flag_NoFrame;
}

QString ExposedOutput::exposedOutputName() const
{
    return m_pPropName->valueText();
}

void ExposedOutput::setRefOutputPort(OutputPort *pOutputPort)
{
    Q_ASSERT(pOutputPort);
    m_pReferencedOutputPort = pOutputPort;
}

void ExposedOutput::serialize(QVariantMap &data, SerializationContext *pContext) const
{
    Q_ASSERT(pContext != nullptr);
    AudioUnit::serialize(data, pContext);
    data["name"] = m_pPropName->value();
}

void ExposedOutput::deserialize(const QVariantMap &data, SerializationContext *pContext)
{
    Q_ASSERT(pContext != nullptr);
    m_pPropName->setValue(data["name"]);
    AudioUnit::deserialize(data, pContext);
}

void ExposedOutput::createProperties()
{
    QtVariantProperty *pRoot = rootProperty();
    m_pPropName = propertyManager()->addProperty(QVariant::String, "Name");
    m_pPropName->setValue("output");

    QObject::connect (propertyManager(), &QtVariantPropertyManager::propertyChanged, [this](QtProperty *pProperty){
        // Update text item with the value for the property.
        QtVariantProperty *pV = dynamic_cast<QtVariantProperty*>(pProperty);
        if (pV == m_pPropName) {
            if (m_pNameItem != nullptr) {
                m_pNameItem->setText(QString(" %1").arg(pV->valueText()));
            }
        }
    });
    pRoot->addSubProperty(m_pPropName);
}
