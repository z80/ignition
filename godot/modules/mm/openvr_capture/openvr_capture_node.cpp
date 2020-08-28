
#include "openvr_capture_node.h"

void OpenvrCaptureNode::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("init", "int"), &OpenvrCaptureNode::init,    Variant::BOOL );
	ClassDB::bind_method( D_METHOD("process"),     &OpenvrCaptureNode::process, Variant::BOOL );
	ClassDB::bind_method( D_METHOD("pose", "int"), &OpenvrCaptureNode::pose,    Variant::TRANSFORM );

}

OpenvrCaptureNode::OpenvrCaptureNode()
	: Node()
{
}

OpenvrCaptureNode::~OpenvrCaptureNode()
{
}

bool OpenvrCaptureNode::init( int qty )
{
	const bool res = _c.init( qty );
	return res;
}

void OpenvrCaptureNode::finit()
{
	_c.finit();
}

bool OpenvrCaptureNode::process()
{
	const bool res = _c.process();
	return res;
}

const Transform & OpenvrCaptureNode::pose( int i ) const
{
	const Transform & t = _c.pose( i );
	return t;
}
