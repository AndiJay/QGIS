#include "vectorlayer3drenderer.h"

#include "abstract3dsymbol.h"
#include "lineentity.h"
#include "pointentity.h"
#include "polygonentity.h"
#include "modelentity.h"

#include "qgsvectorlayer.h"
#include "qgsxmlutils.h"


VectorLayer3DRendererMetadata::VectorLayer3DRendererMetadata()
  : Qgs3DRendererAbstractMetadata( "vector" )
{
}

QgsAbstract3DRenderer *VectorLayer3DRendererMetadata::createRenderer( QDomElement &elem, const QgsReadWriteContext &context )
{
  VectorLayer3DRenderer *r = new VectorLayer3DRenderer;
  r->readXml( elem, context );
  return r;
}


// ---------


VectorLayer3DRenderer::VectorLayer3DRenderer( Abstract3DSymbol *s )
  : mSymbol( s )
{
}

VectorLayer3DRenderer::~VectorLayer3DRenderer()
{
}

VectorLayer3DRenderer *VectorLayer3DRenderer::clone() const
{
  VectorLayer3DRenderer *r = new VectorLayer3DRenderer( mSymbol ? mSymbol->clone() : nullptr );
  r->layerRef = layerRef;
  return r;
}

void VectorLayer3DRenderer::setLayer( QgsVectorLayer *layer )
{
  layerRef = QgsMapLayerRef( layer );
}

QgsVectorLayer *VectorLayer3DRenderer::layer() const
{
  return qobject_cast<QgsVectorLayer *>( layerRef.layer );
}

void VectorLayer3DRenderer::setSymbol( Abstract3DSymbol *symbol )
{
  mSymbol.reset( symbol );
}

const Abstract3DSymbol *VectorLayer3DRenderer::symbol() const
{
  return mSymbol.get();
}

QVector<Qt3DCore::QEntity *> VectorLayer3DRenderer::createEntities( const Map3D &map ) const
{
  QVector<Qt3DCore::QEntity *> entities;

  QgsVectorLayer *vl = layer();

  if ( mSymbol && vl )
  {
      if ( mSymbol->type() == "polygon" )
        entities.push_back(new PolygonEntity( map, vl, *static_cast<Polygon3DSymbol *>( mSymbol.get() ) ));
      else if ( mSymbol->type() == "point" )
      {
        Point3DSymbol* pointSymbol = static_cast<Point3DSymbol *>( mSymbol.get() );
        QString shape = pointSymbol->shapeProperties["shape"].toString();
        if ( shape == "model" ) {
            QList<QVector3D> positions = Utils::positions(map, vl);
            Q_FOREACH(const QVector3D& position, positions) {
                entities.push_back(new ModelEntity(position, *pointSymbol ));
            }
        }
        else {
            entities.push_back(new PointEntity( map, vl, *pointSymbol  ));
        }
      }
      else if ( mSymbol->type() == "line" )
        entities.push_back(new LineEntity( map, vl, *static_cast<Line3DSymbol *>( mSymbol.get() ) ));
  }
  return entities;
}

void VectorLayer3DRenderer::writeXml( QDomElement &elem, const QgsReadWriteContext &context ) const
{
  QDomDocument doc = elem.ownerDocument();

  elem.setAttribute( "layer", layerRef.layerId );

  QDomElement elemSymbol = doc.createElement( "symbol" );
  if ( mSymbol )
  {
    elemSymbol.setAttribute( "type", mSymbol->type() );
    mSymbol->writeXml( elemSymbol, context );
  }
  elem.appendChild( elemSymbol );
}

void VectorLayer3DRenderer::readXml( const QDomElement &elem, const QgsReadWriteContext &context )
{
  layerRef = QgsMapLayerRef( elem.attribute( "layer" ) );

  QDomElement elemSymbol = elem.firstChildElement( "symbol" );
  QString symbolType = elemSymbol.attribute( "type" );
  Abstract3DSymbol *symbol = nullptr;
  if ( symbolType == "polygon" )
    symbol = new Polygon3DSymbol;
  else if ( symbolType == "point" )
    symbol = new Point3DSymbol;
  else if ( symbolType == "line" )
    symbol = new Line3DSymbol;

  if ( symbol )
    symbol->readXml( elemSymbol, context );
  mSymbol.reset( symbol );
}

void VectorLayer3DRenderer::resolveReferences( const QgsProject &project )
{
  layerRef.setLayer( project.mapLayer( layerRef.layerId ) );
}
