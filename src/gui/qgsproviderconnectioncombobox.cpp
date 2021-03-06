/***************************************************************************
   qgsproviderconnectioncombobox.cpp
    --------------------------------
   Date                 : March 2020
   Copyright            : (C) 2020 Nyall Dawson
   Email                : nyall dot dawson at gmail dot com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "qgsproviderconnectioncombobox.h"
#include "qgsproviderconnectionmodel.h"

QgsProviderConnectionComboBox::QgsProviderConnectionComboBox( const QString &provider, QWidget *parent )
  : QComboBox( parent )
{
  mModel = new QgsProviderConnectionModel( provider, this );

  mSortModel = new QSortFilterProxyModel( this );
  mSortModel->setSourceModel( mModel );
  mSortModel->setSortRole( Qt::DisplayRole );
  mSortModel->setSortLocaleAware( true );
  mSortModel->setSortCaseSensitivity( Qt::CaseInsensitive );
  mSortModel->setDynamicSortFilter( true );
  mSortModel->sort( 0 );

  setModel( mSortModel );

  connect( this, static_cast < void ( QComboBox::* )( int ) > ( &QComboBox::activated ), this, &QgsProviderConnectionComboBox::indexChanged );
  connect( mSortModel, &QAbstractItemModel::rowsInserted, this, &QgsProviderConnectionComboBox::rowsChanged );
  connect( mSortModel, &QAbstractItemModel::rowsRemoved, this, &QgsProviderConnectionComboBox::rowsChanged );
}

void QgsProviderConnectionComboBox::setConnection( const QString &connection )
{
  if ( connection == currentConnection() )
    return;

  if ( connection.isEmpty() )
  {
    setCurrentIndex( -1 );
    emit connectionChanged( QString() );
    return;
  }

  QModelIndexList idx = mSortModel->match( mSortModel->index( 0, 0 ), QgsProviderConnectionModel::RoleConnectionName, connection, Qt::MatchFixedString | Qt::MatchCaseSensitive );
  if ( !idx.empty() )
  {
    QModelIndex proxyIdx = idx.at( 0 );
    if ( proxyIdx.isValid() )
    {
      setCurrentIndex( proxyIdx.row() );
      emit connectionChanged( currentConnection() );
      return;
    }
  }
  setCurrentIndex( -1 );
  emit connectionChanged( QString() );
}

QString QgsProviderConnectionComboBox::currentConnection() const
{
  const QModelIndex proxyIndex = mSortModel->index( currentIndex(), 0 );
  if ( !proxyIndex.isValid() )
  {
    return QString();
  }

  return mSortModel->data( proxyIndex, QgsProviderConnectionModel::RoleConnectionName ).toString();
}

QString QgsProviderConnectionComboBox::currentConnectionUri() const
{
  const QModelIndex proxyIndex = mSortModel->index( currentIndex(), 0 );
  if ( !proxyIndex.isValid() )
  {
    return QString();
  }

  return mSortModel->data( proxyIndex, QgsProviderConnectionModel::RoleUri ).toString();
}

void QgsProviderConnectionComboBox::indexChanged( int i )
{
  Q_UNUSED( i )
  emit connectionChanged( currentConnection() );
}

void QgsProviderConnectionComboBox::rowsChanged()
{
  if ( count() == 1 )
  {
    //currently selected connection item has changed
    emit connectionChanged( currentConnection() );
  }
  else if ( count() == 0 )
  {
    emit connectionChanged( QString() );
  }
}
