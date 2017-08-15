#ifndef QGS3DUTILS_H
#define QGS3DUTILS_H

class QgsLineString;
class QgsPolygonV2;

#include "qgs3dmapsettings.h"
#include "aabb.h"

//! how to handle altitude of vector features
enum AltitudeClamping
{
  AltClampAbsolute,   //!< Z_final = z_geometry
  AltClampRelative,   //!< Z_final = z_terrain + z_geometry
  AltClampTerrain,    //!< Z_final = z_terrain
};


//! how to handle clamping of vertices of individual features
enum AltitudeBinding
{
  AltBindVertex,      //!< Clamp every vertex of feature
  AltBindCentroid,    //!< Clamp just centroid of feature
};


class _3D_EXPORT Qgs3DUtils
{
  public:

    /**
     * Calculates the highest needed zoom level for tiles in quad-tree given width of the base tile (zoom level 0)
     * in map units, resolution of the tile (e.g. tile's texture width) and desired maximum error in map units.
     */
    static int maxZoomLevel( double tile0width, double tileResolution, double maxError );

    static QString altClampingToString( AltitudeClamping altClamp );
    static AltitudeClamping altClampingFromString( const QString &str );

    static QString altBindingToString( AltitudeBinding altBind );
    static AltitudeBinding altBindingFromString( const QString &str );

    static void clampAltitudes( QgsLineString *lineString, AltitudeClamping altClamp, AltitudeBinding altBind, const QgsPoint &centroid, float height, const Qgs3DMapSettings &map );
    static bool clampAltitudes( QgsPolygonV2 *polygon, AltitudeClamping altClamp, AltitudeBinding altBind, float height, const Qgs3DMapSettings &map );

    static QString matrix4x4toString( const QMatrix4x4 &m );
    static QMatrix4x4 stringToMatrix4x4( const QString &str );

    /**
     * Calculates (x,y,z) position of point in the Point vector layers
     */
    static QList<QVector3D> positions( const Qgs3DMapSettings &map, QgsVectorLayer *layer, const QgsFeatureRequest &req );

    /**
        Returns true if bbox is completely outside the current viewing volume.
        This is used to perform object culling checks.
    */
    static bool isCullable( const AABB &bbox, const QMatrix4x4 &viewProjectionMatrix );

};

#endif // QGS3DUTILS_H
