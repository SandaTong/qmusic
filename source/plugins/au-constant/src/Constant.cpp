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
#include "Constant.h"

const QColor cItemColor(255, 220, 255);

Constant::Constant(AudioUnitPlugin *pPlugin)
    : AudioUnit(pPlugin)
{
    m_pOutput = addOutput();
    createProperties();

    m_pValueItem = nullptr;
}

Constant::~Constant()
{
}

void Constant::processStart()
{
    m_pOutput->setValue(m_pPropConstant->value().toFloat());
}

void Constant::processStop()
{
}

void Constant::process()
{
    // Constant does not change while processing
}

void Constant::reset()
{
}

QGraphicsItem* Constant::graphicsItem()
{
    if (m_pValueItem == nullptr) {
        m_pValueItem = new QGraphicsSimpleTextItem();
        m_pValueItem->setText(QString::number(m_pPropConstant->value().toFloat(), 'f', 2));
    }
    return m_pValueItem;
}

QColor Constant::color() const
{
    return cItemColor;
}

int Constant::flags() const
{
    return Flag_NoTitle;
}

void Constant::serialize(QVariantMap &data, SerializationContext *pContext) const
{
    Q_ASSERT(pContext != nullptr);
    AudioUnit::serialize(data, pContext);
    data["value"] = m_pPropConstant->value();
}

void Constant::deserialize(const QVariantMap &data, SerializationContext *pContext)
{
    Q_ASSERT(pContext != nullptr);
    m_pPropConstant->setValue(data["value"]);
    AudioUnit::deserialize(data, pContext);
}

void Constant::createProperties()
{
    QtVariantProperty *pRoot = rootProperty();
    m_pPropConstant = propertyManager()->addProperty(QVariant::Double, "Value");
    m_pPropConstant->setValue(0.0);
    m_pPropConstant->setAttribute("decimals", 3);
    m_pPropConstant->setAttribute("singleStep", 0.1);

    QObject::connect (propertyManager(), &QtVariantPropertyManager::propertyChanged, [this](QtProperty *pProperty){
        // Update text item with the value for the property.
        QtVariantProperty *pV = dynamic_cast<QtVariantProperty*>(pProperty);
        if (pV == m_pPropConstant) {
            if (m_pValueItem != nullptr) {
                m_pValueItem->setText(QString::number(pV->value().toFloat(), 'f', 2));

                // Update the output immediately
                // TODO: This operation in not atomic
                m_pOutput->setValue(pV->value().toFloat());
            }
        }
    });
    pRoot->addSubProperty(m_pPropConstant);
}
