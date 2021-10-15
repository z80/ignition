
#include "workshop.h"
#include "Urho3D/Core/CoreEvents.h"
#include "Urho3D/Input/InputEvents.h"
#include "Urho3D/UI/UIEvents.h"

#include "Global.h"
#include "MyEvents.h"
#include "Audio/AudioManagerDefs.h"
#include "Messages/Achievements.h"
#include "Input/ControllerInput.h"
#include "LevelManager.h"

#include "name_generator.h"
#include "game_data.h"
#include "design_manager.h"
#include "block.h"
#include "camera_orb_2.h"
#include "physics_events_2.h"

#include <iostream>

using namespace Urho3D;

namespace Osp
{

URHO3D_EVENT( E_CATEGORY_CLICKED, CategoryClicked )
{
    URHO3D_PARAM( P_INDEX, index ); // category index
}

URHO3D_EVENT( E_CREATE_BLOCK_CLICKED, CreateBlockClicked )
{
    URHO3D_PARAM( P_NAME, name ); // Block type name.
}


Workshop::Workshop(Context* context)
    : BaseLevel(context),
      mode( None ),
      mouseX(0),
      mouseY(0),
      mouseX_0(0),
      mouseY_0(0)
{
}

Workshop::~Workshop()
{
    scene_.Reset();
}

void Workshop::Init()
{
    if (!scene_)
    {
        // There is no scene, get back to the main menu
        VariantMap& eventData = GetEventDataMap();
        eventData["Name"] = "MainMenu";
        SendEvent(MyEvents::E_SET_LEVEL, eventData);

        return;
    }

    BaseLevel::Init();

    techTree = GetSubsystem<TechTree>();

    // Disable achievement showing for this level
    GetSubsystem<Achievements>()->SetShowAchievements(true);

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateUI();

    // Subscribing to events.
    SubscribeToEvents();

//    data["Title"] = "Hey!";
//    data["Message"] = "Seems like everything is ok!";
//    SendEvent("ShowAlertMessage", data);
}

void Workshop::Finit()
{
    if ( rootNode )
        rootNode->Remove();
    if ( _panelTech )
        _panelTech->Remove();
    if ( _modeText )
        _modeText->Remove();
    if ( _auxPanel )
        _auxPanel->Remove();
    if ( _techPanel )
        _techPanel->Remove();
}

void Workshop::CreateScene()
{
    Input * inp = GetSubsystem<Input>();
    inp->SetMouseVisible( true );

    ControllerInput * controllerInput = GetSubsystem<ControllerInput>();
    Vector<int> controlIndexes = controllerInput->GetControlIndexes();
    InitViewports(controlIndexes);

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "Prefabs/Workshop.xml" );
    if ( !f )
        return;
    //const bool loadedOk = scene_->LoadXML( f->GetRoot() );
    rootNode = scene_->CreateChild( "Workshop" );
    const bool loadedOk = rootNode->LoadXML( f->GetRoot() );
    //rootNode = scene_->GetChild( "Workshop", true );

    Node * camNode = _cameras[0];
    camNode->SetParent( rootNode );

    CameraOrb2 * camCtrl = camNode->GetOrCreateComponent<CameraOrb2>();
    //camCtrl->updateCamera();
}

void Workshop::CreateUI()
{
    createSectionsUi();
    createBlocksUi(0);
    createModeUi();
    createAuxilaryPanel();
}

void Workshop::createTechPanel()
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();
    UIElement * pnl = root->GetChild( "TechPanel", true );
    if ( pnl )
        return;

    /*ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "UI/TechPanel.xml" );
    if ( !f )
        return;

    SharedPtr<UIElement> panel = ui->LoadLayout( f );
    root->AddChild( panel );*/

    Window * panel = root->CreateChild<Window>( "TechPanel" );
    _techPanel = SharedPtr<Window>( panel );
    panel->SetStyleAuto();
    panel->SetAlignment( HA_LEFT, VA_TOP );
    panel->SetSize( 128, 512 );
    panel->SetLayout( LM_HORIZONTAL );
    panel->SetLayoutBorder( IntRect( 5, 5, 5 ,5 ) );
    panel->SetLayoutSpacing( 5 );

    Window * p = panel->CreateChild<Window>( "Categories" );
    p->SetStyleAuto();
    p->SetAlignment( HA_LEFT, VA_TOP );
    p->SetSize( 64, 512 );
    p->SetLayout( LM_VERTICAL );
    p->SetLayoutBorder( IntRect( 5, 5, 5 ,5 ) );
    p->SetLayoutSpacing( 5 );

    p = panel->CreateChild<Window>( "Blocks" );
    p->SetStyleAuto();
    p->SetAlignment( HA_LEFT, VA_TOP );
    p->SetSize( 64, 512 );
    p->SetLayout( LM_VERTICAL );
    p->SetLayoutBorder( IntRect( 5, 5, 5 ,5 ) );
    p->SetLayoutSpacing( 5 );
}

void Workshop::createSectionsUi()
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();

    Input* input = GetSubsystem<Input>();
    if ( !input->IsMouseVisible() )
        input->SetMouseVisible(true);

    createTechPanel();
    UIElement * p = root->GetChild( "Categories", true );
    if ( !p )
        return;

    // Remove all existing categories.
    //p->RemoveAllChildren();
    {
        const Vector<SharedPtr<UIElement> > & children = p->GetChildren();
        const size_t qty = children.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            const SharedPtr<UIElement> & e = children[i];
            if ( !e )
                continue;
            Button * b = e->Cast<Button>();
            if ( !b )
                continue;
            b->Remove();;
        }
    }

    // Create categories.
    std::vector<CategoryDesc> & cats = techTree->getPanelContent();
    const size_t qty = cats.size();
    for ( size_t i=0; i<qty; i++ )
    {
        const CategoryDesc & c = cats[i];

        Button * b = new Button( context_ );
        b->SetStyleAuto();
        b->SetPosition( 0, 0 );
        b->SetMinSize( 48, 48 );
        b->SetMaxSize( 48, 48 );
        p->AddChild( b );

        SubscribeToEvent( b, E_RELEASED,
            std::bind( &Workshop::HandlePanelGroupClicked,
                       this, i ) );
    }

    return;

}

void Workshop::createBlocksUi( int groupInd )
{
    std::vector<CategoryDesc> & cats = techTree->getPanelContent();
    const size_t qty = cats.size();
    if ( groupInd >= qty )
        return;

    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();

    Input* input = GetSubsystem<Input>();
    if ( !input->IsMouseVisible() )
        input->SetMouseVisible(true);

    createTechPanel();
    UIElement * p = root->GetChild( "Blocks", true );
    if ( !p )
        return;

    // Remove all existing blocks.
    //p->RemoveAllChildren();
    {
        const Vector<SharedPtr<UIElement> > & children = p->GetChildren();
        const size_t qty = children.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            const SharedPtr<UIElement> & e = children[0];
            if ( !e )
                continue;
            Button * b = e->Cast<Button>();
            if ( !b )
                continue;
            b->Remove();
        }
    }

    const std::vector<PartDesc> & blockDescs = techTree->getPartDescs();
    const CategoryDesc & c = cats[groupInd];
    const size_t typesQty = c.items.size();
    for ( size_t i=0; i<typesQty; i++ )
    {
        const int ind = c.items[i];
        const PartDesc & pd = blockDescs[ind];

        Button * b = new Button( context_ );
        b->SetStyleAuto();
        b->SetPosition( 0, 0 );
        b->SetMinSize( 48, 48 );
        b->SetMaxSize( 48, 48 );
        p->AddChild( b );
        SubscribeToEvent( b, E_RELEASED,
                          std::bind( &Workshop::HandlePanelBlockClicked,
                                     this, pd.name ) );
    }
}

void Workshop::createModeUi()
{
    Graphics * graphics = GetSubsystem<Graphics>();
    const int w = graphics->GetWidth();
    const int h = graphics->GetHeight();

    UIElement * root = GetSubsystem<UI>()->GetRoot();

    Text * t = root->CreateChild<Text>();
    t->SetStyleAuto();
    t->SetEditable( false );
    t->SetPosition( w / 2, h * 8 / 10 );
    t->SetText( "Hello!" );

    _modeText = t;
}

void Workshop::SubscribeToEvents()
{
    SubscribeToEvent(E_PHYSICSPRESTEP_2, URHO3D_HANDLER( Workshop, HandlePhysicsPreStep) );
    SubscribeToEvent(E_POSTUPDATE,       URHO3D_HANDLER( Workshop, HandlePostUpdate) );

    SubscribeToEvent( E_MOUSEBUTTONDOWN, URHO3D_HANDLER( Workshop, HandleMouseDown ) );
    SubscribeToEvent( E_MOUSEBUTTONUP,   URHO3D_HANDLER( Workshop, HandleMouseUp ) );
    SubscribeToEvent( E_MOUSEMOVE,       URHO3D_HANDLER( Workshop, HandleMouseMove ) );
    SubscribeToEvent( E_KEYDOWN,         URHO3D_HANDLER( Workshop, HandleKeyDown ) );
    SubscribeToEvent( E_KEYUP,           URHO3D_HANDLER( Workshop, HandleKeyUp ) );

    SubscribeToEvent( E_CATEGORY_CLICKED,     URHO3D_HANDLER( Workshop, HandlePanelGroupSelected ) );
    SubscribeToEvent( E_CREATE_BLOCK_CLICKED, URHO3D_HANDLER( Workshop, HandlePanelBlockSelected ) );
}

Button* Workshop::CreateButton(const String& text, int width, IntVector2 position)
{
    auto* cache = GetSubsystem<ResourceCache>();
    auto* font = cache->GetResource<Font>(APPLICATION_FONT);

    auto* button = GetSubsystem<UI>()->GetRoot()->CreateChild<Button>();
    button->SetStyleAuto();
    button->SetFixedWidth(width);
    button->SetFixedHeight(30);
    button->SetPosition(position);

    auto* buttonText = button->CreateChild<Text>();
    buttonText->SetFont(font, 12);
    buttonText->SetAlignment(HA_CENTER, VA_CENTER);
    buttonText->SetText(text);

    return button;
}

Design Workshop::design()
{
    // First need to traverse all children and choose
    // ones which can be casted to "block" type.
    PODVector<Node*> allCh = rootNode->GetChildren( true );
    const size_t allChQty = allCh.Size();
    typedef std::map<Block *, size_t> BlockInds;
    // For connectivity use a map.
    BlockInds blockInds;
    Vector<SharedPtr<Component> > comps;
    size_t ind = 0;
    // Just walk over all the blocks and generate connections.
    // Design validity is up to the design itself.
    Design d;
    Vector<Block *> blocks;
    for ( size_t i=0; i<allChQty; i++ )
    {
        Node * n = allCh[i];
        comps = n->GetComponents();
        const size_t compsQty = comps.Size();
        for ( size_t j=0; j<compsQty; j++ )
        {
            Block * b = comps[j]->Cast<Block>();
            if ( !b )
                continue;
            // Save inds and blocks.
            blockInds[b] = ind;
            blocks.Push( b );
            ind += 1;
            break;
        }
    }

    const size_t blocksQty = (size_t)ind;

    d.blocks.reserve( blocksQty );
    for ( size_t i=0; i<blocksQty; i++ )
    {
        Block * block = blocks[i];
        Design::Block db;
        db.typeName = block->name;
        block->relativePose( rootNode, db.r, db.q );
        d.blocks.push_back( db );

        // Now need to make connections.
        Block * parentBlock = block->parentBlock();
        if ( !parentBlock )
            continue;
        BlockInds::const_iterator itA = blockInds.find( block );
        if ( itA == blockInds.end() )
            continue;
        BlockInds::const_iterator itB = blockInds.find( parentBlock );
        if ( itB == blockInds.end() )
            continue;
        const size_t indA = itA->second;
        const size_t indB = itB->second;
        // Need to figure out slots blocks are connected through.
        const size_t pivotsQty = block->pivots.size();
        int slotA = -1;
        int slotB = -1;
        for ( size_t j=0; j<pivotsQty; j++ )
        {
            PivotMarker * pm = block->pivots[j];
            Block * connB = pm->blockConnectedTo();
            if ( connB == parentBlock )
            {
                slotA = j;
                const size_t parentPivotsQty = parentBlock->pivots.size();
                for ( size_t k=0; k<parentPivotsQty; k++ )
                {
                    PivotMarker * ppm = parentBlock->pivots[k];
                    Block * pconnB = ppm->blockConnectedTo();
                    if ( pconnB == block )
                    {
                        slotB = k;
                        break;
                    }
                }
                break;
            }
        }

        assert( slotA >= 0 );
        assert( slotB >= 0 );

        Design::Joint j;
        j.blockA = indA;
        j.blockB = indB;
        j.slotA  = slotA;
        j.slotB  = slotB;
        d.joints.push_back( j );
    }

    return d;
}

void Workshop::clearDesign()
{
    // First need to traverse all children and choose
    // ones which can be casted to "block" type.
    PODVector<Node*> allCh = rootNode->GetChildren( true );
    const size_t allChQty = allCh.Size();
    // Just walk over all the blocks and generate connections.
    // Design validity is up to the design itself.
    Design d;
    Vector< SharedPtr<Node> > blocks;
    Vector<SharedPtr<Component> > comps;
    for ( size_t i=0; i<allChQty; i++ )
    {
        Node * n = allCh[i];
        comps = n->GetComponents();
        const size_t compsQty = comps.Size();
        for ( size_t j=0; j<compsQty; j++ )
        {
            Block * b = comps[j]->Cast<Block>();
            if ( !b )
                continue;
            // Save inds and blocks.
            blocks.Push( SharedPtr<Node>( b->GetNode() ) );
            break;
        }
    }

    const size_t blocksQty = blocks.Size();
    for ( size_t i=0; i<blocksQty; i++ )
    {
        SharedPtr<Node> n = blocks[i];
        if (n)
            n->Remove();
    }
}

void Workshop::setDesign( const Design & d )
{
    rootNode = scene_->GetChild( "Workshop", true );

    const size_t blocksQty = d.blocks.size();
    Vector<Block *> blocks;
    blocks.Reserve( blocksQty );
    for ( size_t i=0; i<blocksQty; i++ )
    {
        const Design::Block & db = d.blocks[i];
        const String typeName = db.typeName;
        // Create a part of this name.
        Node * n = rootNode->CreateChild( NameGenerator::Next( typeName ) );
        std::cout << "new block node name: " << n->GetName().CString() << std::endl;
        Object * o = n->CreateComponent( StringHash( typeName ) );
        if ( !o )
            return;

        Block * b = o->Cast<Block>();
        b->setR( db.r );
        b->setQ( db.q );

        blocks.Push( b );
    }

    const size_t jointsQty = d.joints.size();
    for ( size_t i=0; i<jointsQty; i++ )
    {
        const Design::Joint & joint = d.joints[i];
        Block * a = blocks[joint.blockA];
        Block * b = blocks[joint.blockB];
        a->setParent( b );
    }

    showPivots( false );
}

bool Workshop::select()
{
    UI * ui = GetSubsystem<UI>();
    IntVector2 pos = ui->GetCursorPosition();
    // Check the cursor is visible and there is no UI element in front of the cursor
    //if ( !ui->GetCursor()->IsVisible() || ui->GetElementAt(pos, true))
    //    return false;

    Graphics * graphics = GetSubsystem<Graphics>();
    Node   * camNode = _cameras[0];
    Camera * camera  = camNode->GetComponent<Camera>();
    Ray cameraRay = camera->GetScreenRay((float)pos.x_ / graphics->GetWidth(), (float)pos.y_ / graphics->GetHeight());

    // Pick only geometry objects, not eg. zones or lights, only get the first (closest) hit
    PODVector<RayQueryResult> results;
    const float maxDistance = 300.0f;
    RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY );
    scene_->GetComponent<Octree>()->RaycastSingle( query );
    const size_t qty = results.Size();
    if ( !qty )
    {
        selectedBlock.Reset();
        hintDefault();
        return false;
    }

    RayQueryResult& result = results[0];
    //hitPos = result.position_;
    Node * n = result.node_;
    std::cout << "node name: " << n->GetName().CString() << std::endl;
    if ( !n )
    {
        selectedBlock.Reset();
        hintDefault();
        return false;
    }

    const Vector<SharedPtr<Component> > & comps = n->GetComponents();
    const size_t compsQty = comps.Size();
    Block * b = nullptr;
    for ( size_t i=0; i<compsQty; i++ )
    {
        b = comps[i]->Cast<Block>();
        if ( b )
            break;
    }
    if ( !b )
    {
        selectedBlock.Reset();
        hintDefault();
        return false;
    }

    selectedBlock = SharedPtr<Block>( b );
    hintSelected();

    return true;
}

void Workshop::cameraPlane( Vector3 & x, Vector3 & y, Vector3 & n )
{
    Node * camNode = _cameras[0];
    Camera * cam = camNode->GetComponent<Camera>();
    Vector3 a( 0.0, 0.0, 1.0 );
    const Quaternion q = camNode->GetRotation();
    a = q * a;
    // Check if "y" abs value is > 0.707 or not.
    const float TH = 0.707f;
    const bool vert = ( std::abs( a.y_ ) <= TH );
    if ( vert )
    {
        x = Vector3( 1.0, 0.0, 0.0 );
        x = q * x;

        y = Vector3( 0.0, 1.0, 0.0 );
        n = x.CrossProduct( y );
        return;
    }
    x = Vector3( 1.0, 0.0, 0.0 );
    x = q * x;
    x = Vector3( x.x_, 0.0, x.z_ );
    x.Normalize();

    n = Vector3( 0.0, 1.0, 0.0 );
    y = n.CrossProduct( x );
}

void Workshop::mouseIntersection( Vector3 & at, const Vector3 & origin )
{
    const float DEG2RAD = 3.1415f / 180.0f;

    UI * ui = GetSubsystem<UI>();
    const IntVector2 pos = ui->GetCursorPosition();

    Graphics * graphics = GetSubsystem<Graphics>();

    Node * camNode = _cameras[0];
    Camera * cam = camNode->GetComponent<Camera>();
    const int w = graphics->GetWidth();
    const int h = graphics->GetHeight();

    /*Ray cameraRay = cam->GetScreenRay((float)pos.x_ / w, (float)pos.y_ / h);

    const float fovX  = cam->GetFov() * DEG2RAD / 2.0f;
    const float ratio = cam->GetAspectRatio();
    const float fovY  = fovX / ratio;
    const float tx = std::tan( fovX );
    const float ty = std::tan( fovY );
    const float ax = float(2*pos.x_ - w) / float( w );
    const float ay = float(h - 2*pos.y_) / float( h );
    Vector3 a( ax*tx, ay*ty, 1.0 );*/

    const Matrix4 projInverse = cam->GetProjection().Inverse();

    float x_ = float(pos.x_) / float(w);
    float y_ = float(pos.y_) / float(h);
    // The parameters range from 0.0 to 1.0. Expand to normalized device coordinates (-1.0 to 1.0) & flip Y axis
    x_ = 2.0f * x_ - 1.0f;
    y_ = 1.0f - 2.0f * y_;
    Vector3 near(x_, y_, 0.0f);
    Vector3 far(x_, y_, 1.0f);

    const Vector3 origin_    = projInverse * near;
    const Vector3 direction_ = ((projInverse * far) - origin_).Normalized();

    Vector3    a = Vector3::ZERO;
    Vector3    rel_r = Vector3::ZERO;
    Quaternion rel_q;
    {
        Node * rootNode = scene_->GetChild( "Workshop" );
        ItemBase::relativePose( camNode, rootNode, rel_r, rel_q );

        a = rel_q*direction_;
        rel_r = rel_r + rel_q*origin_;
    }

    Vector3 x, y, n;
    cameraPlane( x, y, n );
    // (a*t + r_rel - origin)*n = 0
    // (a,n)*t = (origin-r_rel,n)
    float t_den = a.DotProduct( n );
    if ( std::abs( t_den ) < 0.001 )
        at = Vector3( 0.0, 0.0, 0.0 );
    const float t = (origin-rel_r).DotProduct( n ) / t_den;
    at = a*t + rel_r;
}

void Workshop::hintDefault()
{
    _modeText->SetText( "Create blocks using side panel.\n"
                        "Select existing blocks with LMB.\n"
                        "Use RMB for context menu." );
}

void Workshop::hintSelected()
{
    _modeText->SetText( "Use \"g\" to drag object and \n"
                        "\"r\" to rotate it. Click LMB "
                        "to finish modification." );
}

void Workshop::hintDragged()
{
    _modeText->SetText( "Object is being dragged. Press "
                        "LMB to finish ot ESC to cancel." );
}

void Workshop::hintRotated()
{
    _modeText->SetText( "Object is being rotated. Press "
                        "LMB to finish ot ESC to cancel." );
}

void Workshop::windowBlockParams()
{
    Graphics * graphics = GetSubsystem<Graphics>();
    const int wi = graphics->GetWidth();
    const int he = graphics->GetHeight();

    UIElement * root = GetSubsystem<UI>()->GetRoot();

    Window * w = root->CreateChild<Window>();
    w->SetStyleAuto();
    w->SetMinSize( 150, 300 );
    w->SetMaxSize( 150, 300 );
    w->SetLayout( LM_VERTICAL );
    w->SetAlignment( HA_RIGHT, VA_TOP );
    w->SetLayoutBorder( IntRect( 5, 5, 5, 5 ) );

    LineEdit * posX = w->CreateChild<LineEdit>();
    posX->SetName( "posX" );
    LineEdit * posY = w->CreateChild<LineEdit>();
    posY->SetName( "posY" );
    LineEdit * posZ = w->CreateChild<LineEdit>();
    posZ->SetName( "posZ" );
}

void Workshop::createAuxilaryPanel()
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    XMLFile * f = cache->GetResource<XMLFile>( "UI/AuxPanel.xml" );
    if ( !f )
        return;

    UI * ui = GetSubsystem<UI>();
    UIElement * uiRoot = ui->GetRoot();

    _auxPanel = ui->LoadLayout( f );
    uiRoot->AddChild( _auxPanel );

    _auxPanel->SetPosition( 0, 0 );
    _auxPanel->SetAlignment( HA_RIGHT, VA_TOP );

    UIElement * save = _auxPanel->GetChild( "SaveDesign", true );
    if ( save )
        SubscribeToEvent( save, E_RELEASED,
                           URHO3D_HANDLER( Workshop, HandleSaveDesignDialog ) );
    UIElement * load = _auxPanel->GetChild( "LoadDesign", true );
    if ( load )
        SubscribeToEvent( load, E_RELEASED,
                           URHO3D_HANDLER( Workshop, HandleOpenDesignDialog ) );

    UIElement * tryBtn = _auxPanel->GetChild( "Try", true );
    if ( tryBtn )
        SubscribeToEvent( tryBtn, E_RELEASED,
                          URHO3D_HANDLER( Workshop, HandleTry ) );
}

void Workshop::showPivots( bool en )
{
    Node * rootNode = scene_->GetChild( "Workshop" );
    if ( !rootNode )
        return;
    PODVector<Node*> allCh = rootNode->GetChildren( true );
    const size_t allChQty = allCh.Size();
    // Just walk over all the blocks and generate connections.
    // Design validity is up to the design itself.
    Vector< SharedPtr<Node> > blocks;
    Vector<SharedPtr<Component> > comps;
    for ( size_t i=0; i<allChQty; i++ )
    {
        Node * n = allCh[i];
        comps = n->GetComponents();
        const size_t compsQty = comps.Size();
        for ( size_t j=0; j<compsQty; j++ )
        {
            Block * b = comps[j]->Cast<Block>();
            if ( !b )
                continue;
            b->setPivotsVisible( en );
            break;
        }
    }
}

void Workshop::drag()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }

    const Vector3 origin = selectedBlock->relRf();
    Vector3 at;
    mouseIntersection( at, origin );

    selectedBlock->setR( Vector3d(at) );
}

void Workshop::dragStart()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }
    hintDragged();
    showPivots( true );

    selectedBlock->detach();
    mode = Drag;
}

void Workshop::dragStop()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }
    hintSelected();
    showPivots( false );

    selectedBlock->tryAttach();

    mode = None;
}

void Workshop::rotate()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }

    Vector3d    r0d;
    Quaterniond rel_qd;
    Vector3    at;
    const bool relPoseOk = selectedBlock->relativePose( rootNode, r0d, rel_qd );
    Vector3    r0( r0d.x_, r0d.y_, r0d.z_ );
    Quaternion rel_q( rel_qd.w_, rel_qd.x_, rel_qd.y_, rel_qd.z_ );

    mouseIntersection( at, r0 );

    const Vector3 r1 = (mouseIntersectionOrig - r0).Normalized();
    const Vector3 r2 = (at - r0).Normalized();
    Quaternion dq, q;
    if ( rotateAttached )
    {
        Graphics * g = GetSubsystem<Graphics>();
        const int w = g->GetWidth();
        const float angle = ((float)mouseX - (float)mouseX_0) / (float)w * 1.0 * 360.0;
        dq.FromAngleAxis( angle, rotAxis );
        q = qOrig * dq;
    }
    else
    {
        dq.FromRotationTo( r1, r2 );
        q = dq * qOrig;
        const float l = q.LengthSquared();
        q.Normalize();
    }
    selectedBlock->setQ( q );
}

void Workshop::rotateStart()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }

    mouseX_0 = mouseX;
    mouseY_0 = mouseY;
    Block * parentBlock = selectedBlock->parentBlock();
    if ( parentBlock )
    {
        const Vector3d a = selectedBlock->axisToParent();
        rotAxis = Vector3( a.x_, a.y_, a.z_ );
        rotateAttached = true;
    }
    else
    {
        Vector3 x, y;
        cameraPlane( x, y, rotAxis );
        Vector3d    r;
        Quaterniond q;
        const bool res = selectedBlock->relativePose( rootNode, r, q );
        Vector3    at( r.x_, r.y_, r.z_ );
        Quaternion rel_q( q.w_, q.x_, q.y_, q.z_ );
        rotAxis = rel_q.Inverse() * rotAxis;
        rotateAttached = false;
    }

    selectedBlock->detach();
    Vector3d    r;
    Quaterniond q;
    const bool res = selectedBlock->relativePose( rootNode, r, q );
    const Vector3 at = Vector3( r.x_, r.y_, r.z_ );
    qOrig = Quaternion( q.w_, q.x_, q.y_, q.z_ );
    mouseIntersection( mouseIntersectionOrig, at );

    hintRotated();
    showPivots( true );

    mode = Rotate;
}

void Workshop::rotateStop()
{
    if ( !selectedBlock )
    {
        dragStop();
        hintDefault();
        return;
    }
    hintSelected();
    showPivots( false );

    selectedBlock->tryAttach();
    mode = None;
}

void Workshop::deleteSelectedBlock()
{
    if ( !selectedBlock )
        return;
    selectedBlock->GetNode()->Remove();
}


void Workshop::HandlePhysicsPreStep( StringHash t, VariantMap & e )
{

}

void Workshop::HandlePostUpdate( StringHash t, VariantMap & e )
{

    if ( mode == Drag )
        drag();
    else if ( mode == Rotate )
        rotate();
}

void Workshop::HandleMouseDown( StringHash t, VariantMap & e )
{
    // Here need to filter out events over UI.
    UI * ui = GetSubsystem<UI>();
    const IntVector2 v = ui->GetCursorPosition();
    /// Return UI element at global screen coordinates. By default returns only input-enabled elements.
    UIElement * w = ui->GetElementAt( v, false );
    if ( w )
        return;


    const int b = e[MouseButtonDown::P_BUTTON].GetInt();
    if ( b == SDL_BUTTON_LEFT )
    {
        // Prepare to select.
    }
    else if ( b == SDL_BUTTON_RIGHT )
    {
        // Context menu???
    }
}

void Workshop::HandleMouseUp( StringHash t, VariantMap & e )
{
    // Here need to filter out events over UI.
    UI * ui = GetSubsystem<UI>();
    const IntVector2 v = ui->GetCursorPosition();
    /// Return UI element at global screen coordinates. By default returns only input-enabled elements.
    UIElement * w = ui->GetElementAt( v, false );
    if ( w )
        return;


    const int b = e[MouseButtonUp::P_BUTTON].GetInt();
    if ( b == SDL_BUTTON_LEFT )
    {
        // Prepare to select.
        if ( mode == None )
            select();
        else if ( mode == Drag )
            dragStop();
        else if ( mode == Rotate )
            rotateStop();
    }
    else if ( b == SDL_BUTTON_RIGHT )
    {
        // Context menu???
    }
}

void Workshop::HandleMouseMove( StringHash t, VariantMap & e )
{
    mouseX = e[MouseMove::P_X].GetInt();
    mouseY = e[MouseMove::P_Y].GetInt();
    if ( mode == Drag )
        drag();
    else if ( mode == Rotate )
        rotate();
}

void Workshop::HandleKeyDown( StringHash t, VariantMap & e )
{
    const int key = e[KeyDown::P_KEY].GetInt();
    if ( selectedBlock )
    {
        if ( key == KEY_G )
            dragStart();
        else if ( key == KEY_R )
            rotateStart();
        else if ( (key == KEY_X) || (key == KEY_DELETE) )
            deleteSelectedBlock();
        else if ( key == KEY_ESCAPE )
        {
            if ( mode == Drag )
                dragStop();
            else if ( mode == Rotate )
                rotateStop();
            else if ( mode == None )
            {
                selectedBlock = nullptr;
                hintDefault();
            }
        }
    }
    if ( key == KEY_ESCAPE )
    {
        // Need to show game menu here.
    }
    if ( key == KEY_F4 )
    {
        VariantMap & data = GetEventDataMap();
        //data[ "Name" ] = "SettingsWindow";
        data[ "Name" ] = "ScoreboardWindow";
        SendEvent( MyEvents::E_OPEN_WINDOW, data );
    }

}

void Workshop::HandleKeyUp( StringHash t, VariantMap & e )
{
    // Do nothing.
}


void Workshop::HandlePanelGroupClicked( int ind )
{
    VariantMap & data = GetEventDataMap();
    data[ "index" ] = ind;
    SendEvent( E_CATEGORY_CLICKED, data );
}

void Workshop::HandlePanelBlockClicked( const String name )
{
    VariantMap & data = GetEventDataMap();
    data[ "name" ] = name;
    SendEvent( E_CREATE_BLOCK_CLICKED, data );
}

void Workshop::HandlePanelGroupSelected( StringHash eventType, VariantMap & eventData )
{
    const Variant v = eventData[ "index" ];
    const int ind = v.GetInt();
    createBlocksUi( ind );
}

void Workshop::HandlePanelBlockSelected( StringHash eventType, VariantMap & eventData )
{
    const Variant v = eventData[ "name" ];
    const String typeName = v.GetString();
    rootNode = scene_->GetChild( "Workshop", true );
    // Create a part of this name.
    Node * n = rootNode->CreateChild( NameGenerator::Next( typeName ) );
    std::cout << "new block node name: " << n->GetName().CString() << std::endl;
    Object * o = n->CreateComponent( StringHash( typeName ) );
    if ( !o )
        return;

    Block * b = o->Cast<Block>();

    selectedBlock = SharedPtr<Block>( b );

    b->setParent( rootNode );
    dragStart();
}

void Workshop::HandleSaveDesignDialog( StringHash eventType, VariantMap & eventData )
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();
    UIElement * e = root->GetChild( "DesignSave", true );
    if ( !e )
    {
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        XMLFile * f = cache->GetResource<XMLFile>( "UI/DesignSave.xml" );
        if ( !f )
            return;

        UI * ui = GetSubsystem<UI>();
        UIElement * uiRoot = ui->GetRoot();

        SharedPtr<UIElement> ee = ui->LoadLayout( f );
        e = ee.Get();
        uiRoot->AddChild( e );

        UIElement * okBtn = e->GetChild( "Ok", true );
        if ( okBtn )
            SubscribeToEvent( okBtn, E_RELEASED, URHO3D_HANDLER( Workshop, HandleSaveDesignOk ) );
        UIElement * cancelBtn = e->GetChild( "Cancel", true );
        if ( cancelBtn )
            SubscribeToEvent( cancelBtn, E_RELEASED, URHO3D_HANDLER( Workshop, HandleSaveDesignCancel ) );

    }

    e->SetVisible( true );
}

void Workshop::HandleSaveDesignOk( StringHash eventType, VariantMap & eventData )
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();
    UIElement * e = root->GetChild( "DesignSave", true );
    if ( !e )
        return;
    e->SetVisible( false );

    LineEdit * tname = e->GetChild( "Name", true )->Cast<LineEdit>();
    LineEdit * tdesc = e->GetChild( "Desc", true )->Cast<LineEdit>();

    const String n = tname->GetText();
    const String d = tdesc->GetText();

    const Design dn = design();

    DesignManager * dm = GetSubsystem<DesignManager>();
    dm->saveDesign( n, d, dn );
}

void Workshop::HandleSaveDesignCancel( StringHash eventType, VariantMap & eventData )
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();
    UIElement * e = root->GetChild( "DesignSave", true );
    if ( e )
        e->SetVisible( false );
}

void Workshop::HandleTry( StringHash eventType, VariantMap & eventData )
{
    GameData * gd = scene_->GetOrCreateComponent<GameData>();
    if ( !gd )
        URHO3D_LOGERROR( "Can\'t get GameData" );

    Design d = design();
    gd->design = d;

    VariantMap & eData = GetEventDataMap();
    eData["Name"] = "OnePlanet";
    SendEvent(MyEvents::E_SET_LEVEL, eData);
}


void Workshop::HandleOpenDesignDialog( StringHash eventType, VariantMap & eventData )
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();
    UIElement * e = root->GetChild( "DesignOpen", true );
    if ( !e )
    {
        ResourceCache * cache = GetSubsystem<ResourceCache>();
        XMLFile * f = cache->GetResource<XMLFile>( "UI/DesignOpen.xml" );
        if ( !f )
            return;

        UI * ui = GetSubsystem<UI>();
        UIElement * uiRoot = ui->GetRoot();

        SharedPtr<UIElement> ee = ui->LoadLayout( f );
        e = ee.Get();
        uiRoot->AddChild( e );

        UIElement * okBtn = e->GetChild( "Ok", true );
        if ( okBtn )
            SubscribeToEvent( okBtn, E_RELEASED, URHO3D_HANDLER( Workshop, HandleOpenDesignOk ) );
        UIElement * cancelBtn = e->GetChild( "Cancel", true );
        if ( cancelBtn )
            SubscribeToEvent( cancelBtn, E_RELEASED, URHO3D_HANDLER( Workshop, HandleOpenDesignCancel ) );
    }

    UIElement * el = e->GetChild( "DesignList", true );
    if ( !el )
        return;
    ListView * l = el->Cast<ListView>();
    if ( !l )
        return;

    // Clean up list view.
    {
        const PODVector<UIElement * > ch = l->GetChildrenWithTag( "DesignItem", true );
        const size_t qty = ch.Size();
        for ( size_t i=0; i<qty; i++ )
        {
            UIElement * e = ch[i];
            e->Remove();
            //l->RemoveChild( e );
            //l->RemoveChild( e );
            //Text * t = e->Cast<Text>();
            //if ( t )
            //    l->RemoveChild( e );
        }
    }


    // Fill designs list with content.
    DesignManager * dm = GetSubsystem<DesignManager>();
    std::vector<String> allNames = dm->designNames();
    const size_t qty = allNames.size();
    for ( size_t i=0; i<qty; i++ )
    {
        Text * t = new Text( context_ );
        const String & name = allNames[i];
        t->SetText( name );
        t->SetMinSize( 128, 24 );
        t->SetStyleAuto();
        //t->SetAttribute( "name", name );
        t->AddTag( "DesignItem" );
        l->AddItem( t );
    }

    for ( size_t i=0; i<qty; i++ )
    {
        UIElement * e = l->GetItem( i );
        SubscribeToEvent( e, E_CLICK, URHO3D_HANDLER( Workshop, HandleDesignSelected ) );
    }

    e->SetVisible( true );
}

void Workshop::HandleDesignSelected( StringHash eventType, VariantMap & eventData )
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();
    UIElement * e = root->GetChild( "DesignOpen", true );
    if ( !e )
        return;

    // Need to provide description in appropriate text area.
    UIElement * desc_ = e->GetChild( "Desc", true );
    if ( !desc_ )
        return;

    LineEdit * desc = desc_->Cast<LineEdit>();
    if ( !desc )
        return;

    UIElement * el = e->GetChild( "DesignList", true );
    if ( !el )
        return;
    ListView * l = el->Cast<ListView>();
    if ( !l )
        return;

    const size_t ind = l->GetSelection();
    DesignManager * dm = GetSubsystem<DesignManager>();
    const DesignManager::DesignItem & di = dm->designItem( ind );
    desc->SetText( di.desc );
}

void Workshop::HandleOpenDesignOk( StringHash eventType, VariantMap & eventData )
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();
    UIElement * e = root->GetChild( "DesignOpen", true );
    if ( !e )
        return;

    UIElement * el = e->GetChild( "DesignList", true );
    if ( !el )
        return;
    ListView * l = el->Cast<ListView>();
    if ( !l )
        return;

    const size_t ind = l->GetSelection();
    DesignManager * dm = GetSubsystem<DesignManager>();
    const Design d = dm->loadDesign( ind );

    e->SetVisible( false );

    // Applying design to the scene.
    clearDesign();
    setDesign( d );
}

void Workshop::HandleOpenDesignCancel( StringHash eventType, VariantMap & eventData )
{
    UI * ui = GetSubsystem<UI>();
    UIElement * root = ui->GetRoot();
    UIElement * e = root->GetChild( "DesignOpen", true );
    if ( !e )
        return;

    e->SetVisible( false );
}






}


