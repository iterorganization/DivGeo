#include "Element.h"
class Element;
DG_DEFINE_PTR(Element)
DG_DEFINE_LIST(Element)

#include "Node.h"
class Node;
DG_DEFINE_PTR(Node)
DG_DEFINE_LIST(Node)
DG_DEFINE_ARRAY(Node)

#include "Template.h"
class Template;
DG_DEFINE_PTR(Template)
struct TemplateParams;

#include "Separator.h"
class Separator;
DG_DEFINE_PTR(Separator)
DG_DEFINE_LIST(Separator)

#include "Chords.h"
class Chord;
class ChordProperties;
DG_DEFINE_PTR(Chord)
DG_DEFINE_LIST(Chord)
