/**
 * juce python - Copyright (c) 2023, Lucio Asnaghi. All rights reserved.
 */

#include "ScriptJuceGraphicsBindings.h"
#include "../utilities/ClassDemangling.h"
#include "../utilities/PythonInterop.h"

//#include "../../values/straw_VariantConverter.h"

#include "../pybind11/operators.h"

#include <functional>
#include <string_view>
#include <tuple>

namespace jucepy::Bindings {

// ============================================================================================

template <template <class> class Class, class... Types>
void registerPoint (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;
    py::object scope;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "Point[" << jucepy::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (scope, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType, ValueType>())
            .def (py::self == py::self)
            .def (py::self != py::self)
            .def ("isOrigin", &T::isOrigin)
            .def ("isFinite", &T::isFinite)
            .def ("getX", &T::getX)
            .def ("getY", &T::getY)
            .def ("setX", &T::setX)
            .def ("setY", &T::setY)
            .def ("withX", &T::withX)
            .def ("withY", &T::withY)
            .def ("setXY", &T::setXY)
            .def ("addXY", &T::addXY)
            .def ("translated", &T::translated)
            .def (py::self + py::self)
            .def (py::self += py::self)
            .def (py::self - py::self)
            .def (py::self -= py::self)
            .def (py::self * py::self)
            .def (py::self *= py::self)
            .def (py::self * float())
            .def (py::self *= float())
            .def (py::self / py::self)
            .def (py::self /= py::self)
            .def (py::self / float())
            .def (py::self /= float())
            .def (-py::self)
            .def ("getDistanceFromOrigin", &T::getDistanceFromOrigin)
            .def ("getDistanceFrom", &T::getDistanceFrom)
            .def ("getDistanceSquaredFromOrigin", &T::getDistanceSquaredFromOrigin)
            .def ("getDistanceSquaredFrom", &T::getDistanceSquaredFrom)
            .def ("getAngleToPoint", &T::getAngleToPoint)
            .def ("rotatedAboutOrigin", &T::rotatedAboutOrigin)
            .def ("getPointOnCircumference", py::overload_cast<float, float>(&T::getPointOnCircumference, py::const_))
            .def ("getPointOnCircumference", py::overload_cast<float, float, float>(&T::getPointOnCircumference, py::const_))
            .def ("getDotProduct", &T::getDotProduct)
            .def ("applyTransform", &T::applyTransform)
            .def ("transformedBy", &T::transformedBy)
            .def ("toInt", &T::toInt)
            .def ("toFloat", &T::toFloat)
            .def ("toDouble", &T::toDouble)
            .def ("roundToInt", &T::roundToInt)
            .def ("toString", &T::toString)
            .def_property("x", &T::getX, &T::setX)
            .def_property("y", &T::getY, &T::setY)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("Point", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerRectangle (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;
    py::object scope;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "Rectangle[" << jucepy::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (scope, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType, ValueType>())
            .def (py::init<ValueType, ValueType, ValueType, ValueType>())
            .def (py::init<Point<ValueType>, Point<ValueType>>())
            .def ("isEmpty", &T::isEmpty)
            .def ("isFinite", &T::isFinite)
            .def ("getX", &T::getX)
            .def ("getY", &T::getY)
            .def ("getWidth", &T::getWidth)
            .def ("getHeight", &T::getHeight)
            .def ("getRight", &T::getRight)
            .def ("getBottom", &T::getBottom)
            .def ("getCentreX", &T::getCentreX)
            .def ("getCentreX", &T::getCentreX)
            .def ("getAspectRatio", &T::getAspectRatio)
            .def ("getPosition", &T::getPosition)
            .def ("setPosition", py::overload_cast<Point<ValueType>> (&T::setPosition))
            .def ("setPosition", py::overload_cast<ValueType, ValueType> (&T::setPosition))
            .def ("getTopLeft", &T::getTopLeft)
            .def ("getTopRight", &T::getTopRight)
            .def ("getBottomLeft", &T::getBottomLeft)
            .def ("getBottomRight", &T::getBottomRight)
            .def ("getHorizontalRange", &T::getHorizontalRange)
            .def ("getVerticalRange", &T::getVerticalRange)
            .def ("setSize", &T::setSize)
            .def ("setBounds", &T::setBounds)
            .def ("setX", &T::setX)
            .def ("setY", &T::setY)
            .def ("setWidth", &T::setWidth)
            .def ("setHeight", &T::setHeight)
            .def ("setCentre", py::overload_cast<Point<ValueType>> (&T::setCentre))
            .def ("setCentre", py::overload_cast<ValueType, ValueType> (&T::setCentre))
            .def ("setHorizontalRange", &T::setHorizontalRange)
            .def ("setVerticalRange", &T::setVerticalRange)
            .def ("withX", &T::withX)
            .def ("withY", &T::withY)
            .def ("withRightX", &T::withRightX)
            .def ("withBottomY", &T::withBottomY)
            .def ("withPosition", py::overload_cast<Point<ValueType>> (&T::withPosition, py::const_))
            .def ("withPosition", py::overload_cast<ValueType, ValueType> (&T::withPosition, py::const_))
            .def ("withZeroOrigin", &T::withZeroOrigin)
            .def ("withCentre", &T::withCentre)
            .def ("withWidth", &T::withWidth)
            .def ("withHeight", &T::withHeight)
            .def ("withSize", &T::withSize)
            .def ("withSizeKeepingCentre", &T::withSizeKeepingCentre)
            .def ("setLeft", &T::setLeft)
            .def ("withLeft", &T::withLeft)
            .def ("setTop", &T::setTop)
            .def ("withTop", &T::withTop)
            .def ("setRight", &T::setRight)
            .def ("withRight", &T::withRight)
            .def ("setBottom", &T::setBottom)
            .def ("withBottom", &T::withBottom)
            .def ("withTrimmedLeft", &T::withTrimmedLeft)
            .def ("withTrimmedRight", &T::withTrimmedRight)
            .def ("withTrimmedTop", &T::withTrimmedTop)
            .def ("withTrimmedBottom", &T::withTrimmedBottom)
            .def ("translate", &T::translate)
            .def ("translated", &T::translated)
            // TODO - operators
            .def ("expand", &T::expand)
            .def ("expanded", py::overload_cast<ValueType> (&T::expanded, py::const_))
            .def ("expanded", py::overload_cast<ValueType, ValueType> (&T::expanded, py::const_))
            .def ("reduce", &T::reduce)
            .def ("reduced", py::overload_cast<ValueType> (&T::reduced, py::const_))
            .def ("reduced", py::overload_cast<ValueType, ValueType> (&T::reduced, py::const_))
            .def ("removeFromTop", &T::removeFromTop)
            .def ("removeFromLeft", &T::removeFromLeft)
            .def ("removeFromRight", &T::removeFromRight)
            .def ("removeFromBottom", &T::removeFromBottom)
            .def ("getConstrainedPoint", &T::getConstrainedPoint)
            .def ("getRelativePoint", &T::template getRelativePoint<float>)
            .def ("getRelativePoint", &T::template getRelativePoint<double>)
            .def ("proportionOfWidth", &T::template proportionOfWidth<float>)
            .def ("proportionOfWidth", &T::template proportionOfWidth<double>)
            .def ("proportionOfHeight", &T::template proportionOfHeight<float>)
            .def ("proportionOfHeight", &T::template proportionOfHeight<double>)
            .def ("getProportion", &T::template getProportion<float>)
            .def ("getProportion", &T::template getProportion<double>)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("Rectangle", type);
}

// ============================================================================================

template <template <class> class Class, class... Types>
void registerBorderSize (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    py::dict type;
    py::object scope;

    ([&]
    {
        using ValueType = Types;
        using T = Class<ValueType>;

        String className;
        className << "BorderSize[" << jucepy::Helpers::demangleClassName (typeid (Types).name()) << "]";

        auto class_ = py::class_<T> (scope, className.toRawUTF8())
            .def (py::init<>())
            .def (py::init<ValueType>())
            .def (py::init<ValueType, ValueType, ValueType, ValueType>())
            .def ("getTop", &T::getTop)
            .def ("getLeft", &T::getLeft)
            .def ("getBottom", &T::getBottom)
            .def ("getRight", &T::getRight)
            .def ("getTopAndBottom", &T::getTopAndBottom)
            .def ("getLeftAndRight", &T::getLeftAndRight)
            .def ("isEmpty", &T::isEmpty)
            .def ("setTop", &T::setTop)
            .def ("setLeft", &T::setLeft)
            .def ("setBottom", &T::setBottom)
            .def ("setRight", &T::setRight)
            .def ("subtractedFrom", py::overload_cast<const Rectangle<ValueType>&> (&T::subtractedFrom, py::const_))
            .def ("subtractFrom", &T::subtractFrom)
            .def ("addedTo", py::overload_cast<const Rectangle<ValueType>&> (&T::addedTo, py::const_))
            .def ("addTo", &T::addTo)
            .def ("subtractedFrom", py::overload_cast<const T&> (&T::subtractedFrom, py::const_))
            .def ("addedTo", py::overload_cast<const T&> (&T::addedTo, py::const_))
            .def ("multipliedBy", &T::template multipliedBy<int>)
            .def ("multipliedBy", &T::template multipliedBy<float>)
            .def (py::self == py::self)
            .def (py::self != py::self)
        ;

        type[py::type::of (typename Helpers::CppToPython<Types>::type{})] = class_;

        return true;
    }() && ...);

    m.add_object ("BorderSize", type);
}

void registerJuceGraphicsBindings (pybind11::module_& m)
{
    using namespace juce;

    namespace py = pybind11;

    // ============================================================================================ juce::AffineTransform

    py::class_<Justification> classJustification (m, "Justification");
    classJustification
        .def (py::init<Justification::Flags>())
        .def (py::init<const Justification&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("getFlags", &Justification::getFlags)
        .def ("testFlags", &Justification::testFlags)
        .def ("getOnlyVerticalFlags", &Justification::getOnlyVerticalFlags)
        .def ("getOnlyHorizontalFlags", &Justification::getOnlyHorizontalFlags)
    //.def ("applyToRectangle", &Justification::applyToRectangle)
    //.def ("appliedToRectangle", &Justification::appliedToRectangle)
    ;

    py::enum_<Justification::Flags> (classJustification, "Flags")
        .value("left", Justification::Flags::left)
        .value("right", Justification::Flags::right)
        .value("horizontallyCentred", Justification::Flags::horizontallyCentred)
        .value("top", Justification::Flags::top)
        .value("bottom", Justification::Flags::bottom)
        .value("verticallyCentred", Justification::Flags::verticallyCentred)
        .value("horizontallyJustified", Justification::Flags::horizontallyJustified)
        .value("centred", Justification::Flags::centred)
        .value("centredLeft", Justification::Flags::centredLeft)
        .value("centredRight", Justification::Flags::centredRight)
        .value("centredTop", Justification::Flags::centredTop)
        .value("centredBottom", Justification::Flags::centredBottom)
        .value("topLeft", Justification::Flags::topLeft)
        .value("topRight", Justification::Flags::topRight)
        .value("bottomLeft", Justification::Flags::bottomLeft)
        .value("bottomRight", Justification::Flags::bottomRight)
        .export_values();

    // ============================================================================================ juce::AffineTransform

    py::class_<AffineTransform> (m, "AffineTransform")
        .def (py::init<>())
    //.def (py::init<const AffineTransform&>())
    //.def (py::init<float, float, float, float, float, float>())
        .def (py::self == py::self)
        .def (py::self != py::self)
    //.def ("transformPoint", &AffineTransform::transformPoint)
    //.def ("transformPoints", &AffineTransform::transformPoints)
        .def ("translated", static_cast<AffineTransform (AffineTransform::*)(float, float) const>(&AffineTransform::translated))
        .def_static ("translation", static_cast<AffineTransform (*)(float, float)>(&AffineTransform::translation))
        .def ("withAbsoluteTranslation", &AffineTransform::withAbsoluteTranslation)
        .def ("rotated", py::overload_cast<float>(&AffineTransform::rotated, py::const_))
        .def ("rotated", py::overload_cast<float, float, float>(&AffineTransform::rotated, py::const_))
        .def_static ("rotation", py::overload_cast<float>(&AffineTransform::rotation))
        .def_static ("rotation", py::overload_cast<float, float, float>(&AffineTransform::rotation))
        .def ("scaled", py::overload_cast<float, float>(&AffineTransform::scaled, py::const_))
        .def ("scaled", py::overload_cast<float>(&AffineTransform::scaled, py::const_))
        .def ("scaled", py::overload_cast<float, float, float, float>(&AffineTransform::scaled, py::const_))
        .def_static ("scale", py::overload_cast<float, float>(&AffineTransform::scale))
        .def_static ("scale", py::overload_cast<float>(&AffineTransform::scale))
        .def_static ("scale", py::overload_cast<float, float, float, float>(&AffineTransform::scale))
        .def ("sheared", &AffineTransform::sheared)
        .def_static ("shear", &AffineTransform::shear)
        .def_static ("verticalFlip", &AffineTransform::verticalFlip)
        .def ("inverted", &AffineTransform::inverted)
    //.def ("fromTargetPoints", &AffineTransform::fromTargetPoints)
        .def ("followedBy", &AffineTransform::followedBy)
        .def ("isIdentity", &AffineTransform::isIdentity)
        .def ("isSingularity", &AffineTransform::isSingularity)
        .def ("isOnlyTranslation", &AffineTransform::isOnlyTranslation)
        .def ("getTranslationX", &AffineTransform::getTranslationX)
        .def ("getTranslationY", &AffineTransform::getTranslationY)
        .def ("getDeterminant", &AffineTransform::getDeterminant)
        .def_property("mat00",
                      [](const AffineTransform& self) { return self.mat00; },
                      [](AffineTransform& self, float v) { self.mat00 = v; })
        .def_property("mat01",
                      [](const AffineTransform& self) { return self.mat01; },
                      [](AffineTransform& self, float v) { self.mat01 = v; })
        .def_property("mat02",
                      [](const AffineTransform& self) { return self.mat02; },
                      [](AffineTransform& self, float v) { self.mat02 = v; })
        .def_property("mat10",
                      [](const AffineTransform& self) { return self.mat10; },
                      [](AffineTransform& self, float v) { self.mat10 = v; })
        .def_property("mat11",
                      [](const AffineTransform& self) { return self.mat11; },
                      [](AffineTransform& self, float v) { self.mat11 = v; })
        .def_property("mat12",
                      [](const AffineTransform& self) { return self.mat12; },
                      [](AffineTransform& self, float v) { self.mat12 = v; })
    ;

    // ============================================================================================ juce::Point<>

    registerPoint<Point, int, float> (m);

    // ============================================================================================ juce::Rectangle<>

    registerRectangle<Rectangle, int, float> (m);

    // ============================================================================================ juce::BorderSize<>

    registerBorderSize<BorderSize, int, float> (m);

    // ============================================================================================ juce::Line<>

    py::class_<Line<float>> (m, "LineFloat")
        .def (py::init<>())
        .def ("getStartX", &Line<float>::getStartX)
        .def ("getStartY", &Line<float>::getStartY)
        .def ("getEndX", &Line<float>::getEndX)
        .def ("getEndY", &Line<float>::getEndY)
        .def ("getStart", &Line<float>::getStart)
        .def ("getEnd", &Line<float>::getEnd)
    //.def ("setStart", &Line<float>::setStart)
    //.def ("setStart", &Line<float>::setStart)
    //.def ("setEnd", &Line<float>::setEnd)
    //.def ("setEnd", &Line<float>::setEnd)
        .def ("reversed", &Line<float>::reversed)
        .def ("applyTransform", &Line<float>::applyTransform)
        .def ("getLength", &Line<float>::getLength)
        .def ("getLengthSquared", &Line<float>::getLengthSquared)
        .def ("isVertical", &Line<float>::isVertical)
        .def ("isHorizontal", &Line<float>::isHorizontal)
        .def ("getAngle", &Line<float>::getAngle)
        .def_static ("fromStartAndAngle", &Line<float>::fromStartAndAngle)
        .def ("toFloat", &Line<float>::toFloat)
        .def ("toDouble", &Line<float>::toDouble)
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("getIntersection", &Line<float>::getIntersection)
    //.def ("intersects", &Line<float>::intersects)
    //.def ("intersects", &Line<float>::intersects)
    //.def ("getPointAlongLine", &Line<float>::getPointAlongLine)
    //.def ("getPointAlongLine", &Line<float>::getPointAlongLine)
        .def ("getPointAlongLineProportionally", &Line<float>::getPointAlongLineProportionally)
        .def ("getDistanceFromPoint", &Line<float>::getDistanceFromPoint)
        .def ("findNearestProportionalPositionTo", &Line<float>::findNearestProportionalPositionTo)
        .def ("findNearestPointTo", &Line<float>::findNearestPointTo)
        .def ("isPointAbove", &Line<float>::isPointAbove)
        .def ("withLengthenedStart", &Line<float>::withLengthenedStart)
        .def ("withShortenedStart", &Line<float>::withShortenedStart)
        .def ("withLengthenedEnd", &Line<float>::withLengthenedEnd)
        .def ("withShortenedEnd", &Line<float>::withShortenedEnd)
    ;

    // ============================================================================================ juce::Path

    py::class_<Path> classPath (m, "Path");
    classPath
        .def (py::init<>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isEmpty", &Path::isEmpty)
        .def ("getBounds", &Path::getBounds)
        .def ("getBoundsTransformed", &Path::getBoundsTransformed)
        .def ("contains", py::overload_cast<float, float, float>(&Path::contains, py::const_))
        .def ("contains", py::overload_cast<Point<float>, float>(&Path::contains, py::const_))
        .def ("intersectsLine", &Path::intersectsLine)
        .def ("getClippedLine", &Path::getClippedLine)
        .def ("getLength", &Path::getLength)
        .def ("getPointAlongPath", &Path::getPointAlongPath)
        .def ("getNearestPoint", &Path::getNearestPoint)
        .def ("clear", &Path::clear)
        .def ("startNewSubPath", py::overload_cast<float, float>(&Path::startNewSubPath))
        .def ("startNewSubPath", py::overload_cast<Point<float>>(&Path::startNewSubPath))
        .def ("closeSubPath", &Path::closeSubPath)
        .def ("lineTo", py::overload_cast<float, float>(&Path::lineTo))
        .def ("lineTo", py::overload_cast<Point<float>>(&Path::lineTo))
        .def ("quadraticTo", py::overload_cast<float, float, float, float>(&Path::quadraticTo))
        .def ("quadraticTo", py::overload_cast<Point<float>, Point<float>>(&Path::quadraticTo))
        .def ("cubicTo", py::overload_cast<float, float, float, float, float, float>(&Path::cubicTo))
        .def ("cubicTo", py::overload_cast<Point<float>, Point<float>, Point<float>>(&Path::cubicTo))
        .def ("getCurrentPosition", &Path::getCurrentPosition)
        .def ("addRectangle", static_cast<void (Path::*)(float, float, float, float)>(&Path::addRectangle))
    //.def ("addRoundedRectangle", py::overload_cast<float, float, float, float, float>(&Path::addRoundedRectangle))
    //.def ("addRoundedRectangle", py::overload_cast<float, float, float, float, float, float>(&Path::addRoundedRectangle))
    //.def ("addRoundedRectangle", py::overload_cast<float, float, float, float, float, float, bool, bool, bool, bool>(&Path::addRoundedRectangle))
        .def ("addTriangle", py::overload_cast<float, float, float, float, float, float>(&Path::addTriangle))
        .def ("addTriangle", py::overload_cast<Point<float>, Point<float>, Point<float>>(&Path::addTriangle))
        .def ("addQuadrilateral", &Path::addQuadrilateral)
        .def ("addEllipse", py::overload_cast<float, float, float, float>(&Path::addEllipse))
        .def ("addEllipse", py::overload_cast<Rectangle<float>>(&Path::addEllipse))
        .def ("addArc", &Path::addArc)
        .def ("addCentredArc", &Path::addCentredArc)
        .def ("addPieSegment", py::overload_cast<float, float, float, float, float, float, float>(&Path::addPieSegment))
        .def ("addPieSegment", py::overload_cast<Rectangle<float>, float, float, float>(&Path::addPieSegment))
        .def ("addLineSegment", &Path::addLineSegment)
        .def ("addArrow", &Path::addArrow)
        .def ("addPolygon", &Path::addPolygon)
        .def ("addStar", &Path::addStar)
        .def ("addBubble", &Path::addBubble)
        .def ("addPath", py::overload_cast<const Path&>(&Path::addPath))
        .def ("addPath", py::overload_cast<const Path&, const AffineTransform&>(&Path::addPath))
        .def ("swapWithPath", &Path::swapWithPath)
        .def ("preallocateSpace", &Path::preallocateSpace)
        .def ("applyTransform", &Path::applyTransform)
        .def ("scaleToFit", &Path::scaleToFit)
    //.def ("getTransformToScaleToFit", py::overload_cast<float, float, float, float, bool, Justification>(&Path::getTransformToScaleToFit))
    //.def ("getTransformToScaleToFit", py::overload_cast<Rectangle<float>, bool, Justification>(&Path::getTransformToScaleToFit))
        .def ("createPathWithRoundedCorners", &Path::createPathWithRoundedCorners)
        .def ("setUsingNonZeroWinding", &Path::setUsingNonZeroWinding)
        .def ("isUsingNonZeroWinding", &Path::isUsingNonZeroWinding)
        .def ("loadPathFromStream", &Path::loadPathFromStream)
        .def ("loadPathFromData", &Path::loadPathFromData)
        .def ("writePathToStream", &Path::writePathToStream)
        .def ("toString", &Path::toString)
        .def ("restoreFromString", &Path::restoreFromString)
    ;

    // ============================================================================================ juce::Path

    py::class_<PathStrokeType> classPathStrokeType (m, "PathStrokeType");
    classPathStrokeType
        .def (py::init<float>())
        .def (py::init<float, PathStrokeType::JointStyle>())
        .def (py::init<float, PathStrokeType::JointStyle, PathStrokeType::EndCapStyle>())
        .def ("createStrokedPath", [](const PathStrokeType& self, Path* destPath, const Path& sourcePath, const AffineTransform& transform, float extraAccuracy)
        {
            self.createStrokedPath (*destPath, sourcePath, transform, extraAccuracy);
        })
        //.def ("createDashedStroke", ...)
        //.def ("createStrokeWithArrowheads", ...)
        .def ("getStrokeThickness", &PathStrokeType::getStrokeThickness)
        .def ("setStrokeThickness", &PathStrokeType::setStrokeThickness)
        .def ("getJointStyle", &PathStrokeType::getJointStyle)
        .def ("setJointStyle", &PathStrokeType::setJointStyle)
        .def ("getEndStyle", &PathStrokeType::getEndStyle)
        .def ("setEndStyle", &PathStrokeType::setEndStyle)
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    py::enum_<PathStrokeType::JointStyle> (classPathStrokeType, "JointStyle")
        .value("mitered", PathStrokeType::mitered)
        .value("curved", PathStrokeType::curved)
        .value("beveled", PathStrokeType::beveled)
        .export_values();

    py::enum_<PathStrokeType::EndCapStyle> (classPathStrokeType, "EndCapStyle")
        .value("butt", PathStrokeType::butt)
        .value("square", PathStrokeType::square)
        .value("rounded", PathStrokeType::rounded)
        .export_values();

    // ============================================================================================ juce::PixelARGB

    py::class_<PixelARGB> (m, "PixelARGB")
        .def (py::init<>())
    //.def (py::init<uint8, uint8, uint8, uint8>())
        .def ("getNativeARGB", &PixelARGB::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelARGB::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelARGB::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelARGB::getEvenBytes)
        .def ("getOddBytes", &PixelARGB::getOddBytes)
        .def ("getAlpha", &PixelARGB::getAlpha)
        .def ("getRed", &PixelARGB::getRed)
        .def ("getGreen", &PixelARGB::getGreen)
        .def ("getBlue", &PixelARGB::getBlue)
    //.def ("set", &PixelARGB::set<?>)
        .def ("setARGB", &PixelARGB::setARGB)
    //.def ("blend", &PixelARGB::blend<?>)
    //.def ("blend", py::overload_cast<PixelRGB>(&PixelARGB::blend))
    //.def ("tween", &PixelARGB::tween<?>)
        .def ("setAlpha", &PixelARGB::setAlpha)
    //.def ("multiplyAlpha", py::overload_cast<float>(&PixelARGB::multiplyAlpha, py::const_))
        .def ("getUnpremultiplied", &PixelARGB::getUnpremultiplied)
        .def ("premultiply", &PixelARGB::premultiply)
        .def ("unpremultiply", &PixelARGB::unpremultiply)
        .def ("desaturate", &PixelARGB::desaturate)
    ;

    py::class_<PixelRGB> (m, "PixelRGB")
        .def (py::init<>())
        .def ("getNativeARGB", &PixelRGB::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelRGB::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelRGB::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelRGB::getEvenBytes)
        .def ("getOddBytes", &PixelRGB::getOddBytes)
        .def ("getAlpha", &PixelRGB::getAlpha)
        .def ("getRed", &PixelRGB::getRed)
        .def ("getGreen", &PixelRGB::getGreen)
        .def ("getBlue", &PixelRGB::getBlue)
    //.def ("set", &PixelRGB::set<?>)
        .def ("setARGB", &PixelRGB::setARGB)
    //.def ("blend", &PixelRGB::blend<?>)
    //.def ("blend", py::overload_cast<PixelRGB>(&PixelRGB::blend))
    //.def ("tween", &PixelRGB::tween<?>)
        .def ("setAlpha", &PixelRGB::setAlpha)
    //.def ("multiplyAlpha", py::overload_cast<float>(&PixelRGB::multiplyAlpha, py::const_))
        .def ("premultiply", &PixelRGB::premultiply)
        .def ("unpremultiply", &PixelRGB::unpremultiply)
        .def ("desaturate", &PixelRGB::desaturate)
    ;

    py::class_<PixelAlpha> (m, "PixelAlpha")
        .def (py::init<>())
        .def ("getNativeARGB", &PixelAlpha::getNativeARGB)
        .def ("getInARGBMaskOrder", &PixelAlpha::getInARGBMaskOrder)
        .def ("getInARGBMemoryOrder", &PixelAlpha::getInARGBMemoryOrder)
        .def ("getEvenBytes", &PixelAlpha::getEvenBytes)
        .def ("getOddBytes", &PixelAlpha::getOddBytes)
        .def ("getAlpha", &PixelAlpha::getAlpha)
        .def ("getRed", &PixelAlpha::getRed)
        .def ("getGreen", &PixelAlpha::getGreen)
        .def ("getBlue", &PixelAlpha::getBlue)
    //.def ("set", &PixelAlpha::set<?>)
        .def ("setARGB", &PixelAlpha::setARGB)
    //.def ("blend", &PixelAlpha::blend<?>)
    //.def ("blend", &PixelAlpha::blend<PixelRGB>)
    //.def ("tween", &PixelAlpha::tween<?>)
        .def ("setAlpha", &PixelAlpha::setAlpha)
    //.def ("multiplyAlpha", py::overload_cast<float>(&PixelAlpha::multiplyAlpha, py::const_))
        .def ("premultiply", &PixelAlpha::premultiply)
        .def ("unpremultiply", &PixelAlpha::unpremultiply)
        .def ("desaturate", &PixelAlpha::desaturate)
    ;

    // ============================================================================================ juce::Colour

    py::class_<Colour> (m, "Colour")
        .def (py::init<>())
    //.def (py::init<uint32>())
    //.def (py::init<uint8, uint8, uint8>())
    //.def (py::init<uint8, uint8, uint8, uint8>())
        .def (py::init<uint8, uint8, uint8, float>())
    //.def (py::init<float, float, float, uint8>())
    //.def (py::init<float, float, float, float>())
    //.def (py::init<PixelARGB>())
    //.def (py::init<PixelRGB>())
    //.def (py::init<PixelAlpha>())
        .def_static ("fromRGB", &Colour::fromRGB)
        .def_static ("fromRGBA", &Colour::fromRGBA)
        .def_static ("fromFloatRGBA", &Colour::fromFloatRGBA)
        .def_static ("fromHSV", &Colour::fromHSV)
        .def_static ("fromHSL", &Colour::fromHSL)
        .def ("getRed", &Colour::getRed)
        .def ("getGreen", &Colour::getGreen)
        .def ("getBlue", &Colour::getBlue)
        .def ("getFloatRed", &Colour::getFloatRed)
        .def ("getFloatGreen", &Colour::getFloatGreen)
        .def ("getFloatBlue", &Colour::getFloatBlue)
        .def ("getPixelARGB", &Colour::getPixelARGB)
        .def ("getNonPremultipliedPixelARGB", &Colour::getNonPremultipliedPixelARGB)
        .def ("getARGB", &Colour::getARGB)
        .def ("getAlpha", &Colour::getAlpha)
        .def ("getFloatAlpha", &Colour::getFloatAlpha)
        .def ("isOpaque", &Colour::isOpaque)
        .def ("isTransparent", &Colour::isTransparent)
        .def ("withAlpha", py::overload_cast<float>(&Colour::withAlpha, py::const_))
        .def ("withMultipliedAlpha", &Colour::withMultipliedAlpha)
        .def ("overlaidWith", &Colour::overlaidWith)
        .def ("interpolatedWith", &Colour::interpolatedWith)
        .def ("getHue", &Colour::getHue)
        .def ("getSaturation", &Colour::getSaturation)
        .def ("getSaturationHSL", &Colour::getSaturationHSL)
        .def ("getBrightness", &Colour::getBrightness)
        .def ("getLightness", &Colour::getLightness)
        .def ("getPerceivedBrightness", &Colour::getPerceivedBrightness)
        .def ("withHue", &Colour::withHue)
        .def ("withSaturation", &Colour::withSaturation)
        .def ("withSaturationHSL", &Colour::withSaturationHSL)
        .def ("withBrightness", &Colour::withBrightness)
        .def ("withLightness", &Colour::withLightness)
        .def ("withRotatedHue", &Colour::withRotatedHue)
        .def ("withMultipliedSaturation", &Colour::withMultipliedSaturation)
        .def ("withMultipliedSaturationHSL", &Colour::withMultipliedSaturationHSL)
        .def ("withMultipliedBrightness", &Colour::withMultipliedBrightness)
        .def ("withMultipliedLightness", &Colour::withMultipliedLightness)
        .def ("brighter", &Colour::brighter)
        .def ("darker", &Colour::darker)
        .def ("contrasting", py::overload_cast<float>(&Colour::contrasting, py::const_))
        .def ("contrasting", py::overload_cast<Colour, float>(&Colour::contrasting, py::const_))
    //.def_static ("contrasting", static_cast<Colour (*)(Colour, Colour)>(&Colour::contrasting)) // Not supported by pybind11
        .def_static ("greyLevel", &Colour::greyLevel)
        .def ("toString", &Colour::toString)
        .def_static ("fromString", &Colour::fromString)
        .def ("toDisplayString", &Colour::toDisplayString)
    ;

    // ============================================================================================ juce::Colour

    py::class_<ColourGradient> (m, "ColourGradient")
        .def (py::init<>())
        .def_static ("vertical", [](Colour c1, float y1, Colour c2, float y2) { return ColourGradient::vertical(c1, y1, c2, y2); })
        .def_static ("horizontal", [](Colour c1, float y1, Colour c2, float y2) { return ColourGradient::horizontal(c1, y1, c2, y2); })
        .def ("clearColours", &ColourGradient::clearColours)
        .def ("addColour", &ColourGradient::addColour)
        .def ("removeColour", &ColourGradient::removeColour)
        .def ("multiplyOpacity", &ColourGradient::multiplyOpacity)
        .def ("getNumColours", &ColourGradient::getNumColours)
        .def ("getColourPosition", &ColourGradient::getColourPosition)
        .def ("getColour", &ColourGradient::getColour)
        .def ("setColour", &ColourGradient::setColour)
        .def ("getColourAtPosition", &ColourGradient::getColourAtPosition)
    //.def ("createLookupTable", &ColourGradient::createLookupTable)
    //.def ("createLookupTable", &ColourGradient::createLookupTable)
        .def ("isOpaque", &ColourGradient::isOpaque)
        .def ("isInvisible", &ColourGradient::isInvisible)
        .def_property("point1",
                      [](const ColourGradient& self) { return self.point1; },
                      [](ColourGradient& self, const Point<float>& v) { self.point1 = v; })
        .def_property("point2",
                      [](const ColourGradient& self) { return self.point2; },
                      [](ColourGradient& self, const Point<float>& v) { self.point2 = v; })
        .def_property("isRadial",
                      [](const ColourGradient& self) { return self.isRadial; },
                      [](ColourGradient& self, bool v) { self.isRadial = v; })
        .def (py::self == py::self)
        .def (py::self != py::self)
    ;

    // ============================================================================================ juce::Image

    py::class_<Image> imageClass (m, "Image");
    imageClass
        .def (py::init<>())
    //.def (py::init<Image::PixelFormat, int, int, bool>())
    //.def (py::init<const Image&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isValid", &Image::isValid)
        .def ("isNull", &Image::isNull)
        .def ("getWidth", &Image::getWidth)
        .def ("getHeight", &Image::getHeight)
        .def ("getBounds", &Image::getBounds)
        .def ("getFormat", &Image::getFormat)
        .def ("isARGB", &Image::isARGB)
        .def ("isRGB", &Image::isRGB)
        .def ("isSingleChannel", &Image::isSingleChannel)
        .def ("hasAlphaChannel", &Image::hasAlphaChannel)
        .def ("clear", &Image::clear)
    //.def ("rescaled", &Image::rescaled)
        .def ("createCopy", &Image::createCopy)
        .def ("convertedToFormat", &Image::convertedToFormat)
        .def ("duplicateIfShared", &Image::duplicateIfShared)
        .def ("getClippedImage", &Image::getClippedImage)
        .def ("getPixelAt", &Image::getPixelAt)
        .def ("setPixelAt", &Image::setPixelAt)
        .def ("multiplyAlphaAt", &Image::multiplyAlphaAt)
        .def ("multiplyAllAlphas", &Image::multiplyAllAlphas)
        .def ("desaturate", &Image::desaturate)
        .def ("moveImageSection", &Image::moveImageSection)
    //.def ("createSolidAreaMask", &Image::createSolidAreaMask)
    //.def ("getProperties", &Image::getProperties)
    //.def ("createLowLevelContext", &Image::createLowLevelContext)
        .def ("getReferenceCount", &Image::getReferenceCount)
    //.def ("getPixelData", &Image::getPixelData)
    ;

    py::enum_<Image::PixelFormat> (imageClass, "PixelFormat")
        .value("UnknownFormat", Image::PixelFormat::UnknownFormat)
        .value("RGB", Image::PixelFormat::RGB)
        .value("ARGB", Image::PixelFormat::ARGB)
        .value("SingleChannel", Image::PixelFormat::SingleChannel)
        .export_values();

    // ============================================================================================ juce::FillType

    py::class_<FillType> (m, "FillType")
        .def (py::init<>())
    //.def (py::init<Colour>())
    //.def (py::init<const ColourGradient&>())
    //.def (py::init<ColourGradient&&>())
    //.def (py::init<const Image&, const AffineTransform&>())
    //.def (py::init<const FillType&>())
    //.def (py::init<FillType&&>())
        .def (py::self == py::self)
        .def (py::self != py::self)
        .def ("isColour", &FillType::isColour)
        .def ("isGradient", &FillType::isGradient)
        .def ("isTiledImage", &FillType::isTiledImage)
        .def ("setColour", &FillType::setColour)
        .def ("setGradient", &FillType::setGradient)
        .def ("setTiledImage", &FillType::setTiledImage)
        .def ("setOpacity", &FillType::setOpacity)
        .def ("getOpacity", &FillType::getOpacity)
        .def ("isInvisible", &FillType::isInvisible)
        .def ("transformed", &FillType::transformed)
        .def_property("colour",
                      [](const FillType& self) { return self.colour; },
                      [](FillType& self, const Colour& v) { self.colour = v; })
        .def_property("gradient",
                      [](const FillType& self) { return self.gradient.get(); },
                      [](FillType& self, ColourGradient* v) { self.gradient = v ? std::make_unique<ColourGradient>(*v) : nullptr; })
        .def_property("image",
                      [](const FillType& self) { return self.image; },
                      [](FillType& self, const Image& v) { self.image = v; })
        .def_property("transform",
                      [](const FillType& self) { return self.transform; },
                      [](FillType& self, const AffineTransform& v) { self.transform = v; })
    ;

    // ============================================================================================ juce::Graphics

    py::class_<Graphics> classGraphics (m, "Graphics");
    classGraphics
        .def (py::init<const Image&>())
        .def ("setColour", &Graphics::setColour)
        .def ("setOpacity", &Graphics::setOpacity)
        .def ("setGradientFill", py::overload_cast<const ColourGradient&> (&Graphics::setGradientFill))
        .def ("setTiledImageFill", &Graphics::setTiledImageFill)
        .def ("setFillType", &Graphics::setFillType)
    //.def ("setFont", py::overload_cast<const Font&>(&Graphics::setFont))
    //.def ("setFont", py::overload_cast<float>(&Graphics::setFont))
    //.def ("getCurrentFont", &Graphics::getCurrentFont)
        .def ("drawSingleLineText", &Graphics::drawSingleLineText)
        .def ("drawMultiLineText", &Graphics::drawMultiLineText)
        .def ("drawText", py::overload_cast<const String&, int, int, int, int, Justification, bool> (&Graphics::drawText, py::const_))
        .def ("drawText", py::overload_cast<const String&, Rectangle<int>, Justification, bool> (&Graphics::drawText, py::const_))
        .def ("drawText", py::overload_cast<const String&, Rectangle<float>, Justification, bool> (&Graphics::drawText, py::const_))
        .def ("drawFittedText", py::overload_cast<const String&, int, int, int, int, Justification, int, float> (&Graphics::drawFittedText, py::const_))
        .def ("drawFittedText", py::overload_cast<const String&, Rectangle<int>, Justification, int, float> (&Graphics::drawFittedText, py::const_))
        .def ("fillAll", py::overload_cast<> (&Graphics::fillAll, py::const_))
        .def ("fillAll", py::overload_cast<Colour> (&Graphics::fillAll, py::const_))
        .def ("fillRect", py::overload_cast<Rectangle<int>> (&Graphics::fillRect, py::const_))
        .def ("fillRect", py::overload_cast<Rectangle<float>> (&Graphics::fillRect, py::const_))
        .def ("fillRect", py::overload_cast<int, int, int, int> (&Graphics::fillRect, py::const_))
        .def ("fillRect", py::overload_cast<float, float, float, float> (&Graphics::fillRect, py::const_))
    //.def ("fillRectList", py::overload_cast<const RectangleList<float>&> (&Graphics::fillRectList, py::const_))
    //.def ("fillRectList", py::overload_cast<const RectangleList<int>&> (&Graphics::fillRectList, py::const_))
        .def ("fillRoundedRectangle", py::overload_cast<float, float, float, float, float> (&Graphics::fillRoundedRectangle, py::const_))
        .def ("fillRoundedRectangle", py::overload_cast<Rectangle<float>, float> (&Graphics::fillRoundedRectangle, py::const_))
        .def ("fillCheckerBoard", &Graphics::fillCheckerBoard)
        .def ("drawRect", py::overload_cast<int, int, int, int, int> (&Graphics::drawRect, py::const_))
        .def ("drawRect", py::overload_cast<float, float, float, float, float> (&Graphics::drawRect, py::const_))
        .def ("drawRect", py::overload_cast<Rectangle<int>, int> (&Graphics::drawRect, py::const_))
        .def ("drawRect", py::overload_cast<Rectangle<float>, float> (&Graphics::drawRect, py::const_))
        .def ("drawRoundedRectangle", py::overload_cast<float, float, float, float, float, float> (&Graphics::drawRoundedRectangle, py::const_))
        .def ("drawRoundedRectangle", py::overload_cast<Rectangle<float>, float, float> (&Graphics::drawRoundedRectangle, py::const_))
        .def ("fillEllipse", py::overload_cast<float, float, float, float> (&Graphics::fillEllipse, py::const_))
        .def ("fillEllipse", py::overload_cast<Rectangle<float>> (&Graphics::fillEllipse, py::const_))
        .def ("drawEllipse", py::overload_cast<float, float, float, float, float> (&Graphics::drawEllipse, py::const_))
        .def ("drawEllipse", py::overload_cast<Rectangle<float>, float> (&Graphics::drawEllipse, py::const_))
        .def ("drawLine", py::overload_cast<float, float, float, float> (&Graphics::drawLine, py::const_))
        .def ("drawLine", py::overload_cast<float, float, float, float, float> (&Graphics::drawLine, py::const_))
        .def ("drawLine", py::overload_cast<Line<float>> (&Graphics::drawLine, py::const_))
        .def ("drawLine", py::overload_cast<Line<float>, float> (&Graphics::drawLine, py::const_))
    //.def ("drawDashedLine", &Graphics::drawDashedLine)
        .def ("drawVerticalLine", &Graphics::drawVerticalLine)
        .def ("drawHorizontalLine", &Graphics::drawHorizontalLine)
        .def ("fillPath", py::overload_cast<const Path&> (&Graphics::fillPath, py::const_))
        .def ("fillPath", py::overload_cast<const Path&, const AffineTransform&> (&Graphics::fillPath, py::const_))
        .def ("strokePath", &Graphics::strokePath)
        .def ("drawArrow", &Graphics::drawArrow)
        .def ("setImageResamplingQuality", &Graphics::setImageResamplingQuality)
        .def ("drawImageAt", &Graphics::drawImageAt)
        .def ("drawImage", py::overload_cast<const Image&, int, int, int, int, int, int, int, int, bool> (&Graphics::drawImage, py::const_))
        .def ("drawImageTransformed", &Graphics::drawImageTransformed)
        .def ("drawImage", py::overload_cast<const Image&, Rectangle<float>, RectanglePlacement, bool> (&Graphics::drawImage, py::const_))
        .def ("drawImageWithin", &Graphics::drawImageWithin)
        .def ("getClipBounds", &Graphics::getClipBounds)
        .def ("clipRegionIntersects", &Graphics::clipRegionIntersects)
        .def ("reduceClipRegion", py::overload_cast<int, int, int, int> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<Rectangle<int>> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<const RectangleList<int>&> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<const Path&, const AffineTransform&> (&Graphics::reduceClipRegion))
        .def ("reduceClipRegion", py::overload_cast<const Image&, const AffineTransform&> (&Graphics::reduceClipRegion))
        .def ("excludeClipRegion", &Graphics::excludeClipRegion)
        .def ("isClipEmpty", &Graphics::isClipEmpty)
        .def ("saveState", &Graphics::saveState)
        .def ("restoreState", &Graphics::restoreState)
        .def ("beginTransparencyLayer", &Graphics::beginTransparencyLayer)
        .def ("endTransparencyLayer", &Graphics::endTransparencyLayer)
        .def ("setOrigin", py::overload_cast<Point<int>> (&Graphics::setOrigin))
        .def ("setOrigin", py::overload_cast<int, int> (&Graphics::setOrigin))
        .def ("addTransform", &Graphics::addTransform)
        .def ("resetToDefaultState", &Graphics::resetToDefaultState)
        .def ("isVectorDevice", &Graphics::isVectorDevice)
    ;

    py::enum_<Graphics::ResamplingQuality> (classGraphics, "ResamplingQuality")
        .value("lowResamplingQuality", Graphics::lowResamplingQuality)
        .value("mediumResamplingQuality", Graphics::mediumResamplingQuality)
        .value("highResamplingQuality", Graphics::highResamplingQuality)
        .export_values();

    //py::class_<Graphics::ScopedSaveState> (classGraphics, "ScopedSaveState")
    //    .def (py::init<>([](Graphics* g) { return Graphics::ScopedSaveState (*g); }))
    //    .def ("__enter__", [] {})
    //    .def ("__exit__", [] {});

}

} // namespace jucepy::Bindings
