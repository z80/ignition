
#ifndef __OPENVR_CAPTURE_NODE_H_
#define __OPENVR_CAPTURE_NODE_H_

#include "scene/main/node.h"
#include "openvr_capture.h"

class OpenvrCaptureNode: public Node
{
	GDCLASS(OpenvrCaptureNode, Node);
	OBJ_CATEGORY("Ignition");

protected:
	static void _bind_methods();

public:
	OpenvrCaptureNode();
	~OpenvrCaptureNode();


	bool init( int qty=1 );
	void finit();
	bool process();

	bool connected( int i );
	String serial( int i );
	bool valid( int i ) const;
	Transform pose( int i ) const;

	String timestamp() const;

private:
	OpenvrCapture _c;
};





#endif

