/***************************************************************************
    qgscalloutsregistry.cpp
    -----------------------
    begin                : July 2019
    copyright            : (C) 2019 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgscalloutsregistry.h"
#include "qgscallout.h"
#include "qgsxmlutils.h"

//
// QgsCalloutAbstractMetadata
//

QgsCalloutWidget *QgsCalloutAbstractMetadata::createCalloutWidget( QgsVectorLayer * )
{
  return nullptr;
}

//
// QgsCalloutMetadata
//

QgsCallout *QgsCalloutMetadata::createCallout( const QVariantMap &properties, const QgsReadWriteContext &context )
{
  return mCreateFunc ? mCreateFunc( properties, context ) : nullptr;
}

QgsCalloutWidget *QgsCalloutMetadata::createCalloutWidget( QgsVectorLayer *vl )
{
  return mWidgetFunc ? mWidgetFunc( vl ) : nullptr;
}


//
// QgsCalloutRegistry
//

QgsCalloutRegistry::QgsCalloutRegistry()
{
  // init registry with known callouts
  addCalloutType( new QgsCalloutMetadata( QStringLiteral( "simple" ), QObject::tr( "Simple lines" ), QgsSimpleLineCallout::create ) );
  addCalloutType( new QgsCalloutMetadata( QStringLiteral( "manhattan" ), QObject::tr( "Manhattan lines" ), QgsManhattanLineCallout::create ) );
}

QgsCalloutRegistry::~QgsCalloutRegistry()
{
  qDeleteAll( mMetadata );
}

bool QgsCalloutRegistry::addCalloutType( QgsCalloutAbstractMetadata *metadata )
{
  if ( !metadata || mMetadata.contains( metadata->name() ) )
    return false;

  mMetadata[metadata->name()] = metadata;
  return true;
}

QgsCallout *QgsCalloutRegistry::createCallout( const QString &name, const QDomElement &element, const QgsReadWriteContext &context ) const
{
  const QVariantMap props = QgsXmlUtils::readVariant( element.firstChildElement() ).toMap();
  return createCallout( name, props, context );
}

QStringList QgsCalloutRegistry::calloutTypes() const
{
  return mMetadata.keys();
}

QgsCalloutAbstractMetadata *QgsCalloutRegistry::calloutMetadata( const QString &name ) const
{
  return mMetadata.value( name );
}

QgsCallout *QgsCalloutRegistry::defaultCallout()
{
  return new QgsManhattanLineCallout();
}

QgsCallout *QgsCalloutRegistry::createCallout( const QString &name, const QVariantMap &properties, const QgsReadWriteContext &context ) const
{
  if ( !mMetadata.contains( name ) )
    return nullptr;

  return mMetadata[name]->createCallout( properties, context );
}
