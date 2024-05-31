#include "Game.h"
#include "Box2DHelper.h"

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
    // Inicialización de la ventana de renderizado
    wnd = new RenderWindow(VideoMode(ancho, alto), titulo);
    wnd->setVisible(true);
    fps = 60;
    wnd->setFramerateLimit(fps);
    frameTime = 1.0f / fps;
    SetZoom(); // Configuración del zoom de la cámara
    InitPhysics(); // Inicialización del mundo físico
}

// Método principal que maneja el bucle del juego
void Game::Loop()
{
    while (wnd->isOpen())
    {
        wnd->clear(clearColor); // Limpia la ventana con un color especificado
        DoEvents(); // Procesa los eventos del sistema
        UpdatePhysics(); // Actualiza la simulación física
        DrawGame(); // Dibuja el juego en la ventana
        wnd->display(); // Muestra la ventana renderizada
    }
}

// Actualiza la simulación física
void Game::UpdatePhysics()
{
    phyWorld->Step(frameTime, 8, 8); // Avanza la simulación física
    phyWorld->ClearForces(); // Limpia las fuerzas aplicadas a los cuerpos
    phyWorld->DebugDraw(); // Dibuja el mundo físico (para depuración)
}

// Dibuja los elementos del juego en la ventana
void Game::DrawGame()
{
    // Dibujo de cañon
    sf::RectangleShape groundShape(sf::Vector2f(50, 5));
    groundShape.setFillColor(sf::Color::Red);
    groundShape.setPosition(10, 75);
    groundShape.setRotation(125);
    wnd->draw(groundShape);
}

// Procesa los eventos del sistema
void Game::DoEvents()
{
    Event evt;
    while (wnd->pollEvent(evt))
    {
        switch (evt.type)
        {
        case Event::Closed:
            wnd->close(); // Cierra la ventana
            break;
        case Event::MouseButtonPressed:
            if (evt.mouseButton.button == Mouse::Left)
            {
                sf::Vector2i mousePos = Mouse::getPosition(*wnd);
                CreatePj(mousePos);
            }
            break;
        }
    }
}

// Configura el área visible en la ventana de renderizado
void Game::SetZoom()
{
    View camara;
    camara.setSize(100.0f, 100.0f); // Tamaño del área visible
    camara.setCenter(50.0f, 50.0f); // Centra la vista en estas coordenadas
    wnd->setView(camara); // Asigna la vista a la ventana
}

// Inicializa el mundo físico y los elementos estáticos del juego
void Game::InitPhysics()
{
    // Inicializa el mundo físico con la gravedad por defecto
    phyWorld = new b2World(b2Vec2(0.0f, 25));

    // Inicializa el renderizador de depuración para el mundo físico
    debugRender = new SFMLRenderer(wnd);
    debugRender->SetFlags(UINT_MAX); // Configura el renderizado para que muestre todo
    phyWorld->SetDebugDraw(debugRender);

    // Crea los elementos estáticos del juego (suelo y paredes)
    b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    groundBody->SetTransform(b2Vec2(50.0f, 100.0f), 0.0f);

    b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    leftWallBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);

    b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    rightWallBody->SetTransform(b2Vec2(100.0f, 50.0f), 0.0f);

    b2Body* topWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    topWallBody->SetTransform(b2Vec2(50.0f, 0.0f), 0.0f);


    // pelota de techo
    b2Body* circuloA = Box2DHelper::CreateCircularDynamicBody(phyWorld, 5, 1.0f, 0.5, 0.1f);
    circuloA->SetTransform(b2Vec2(50.0f, 40.0f), 0.0f);

    Box2DHelper::CreateDistanceJoint(phyWorld, circuloA, circuloA->GetWorldCenter(),
        topWallBody, topWallBody->GetWorldCenter(), 10.0f, 0.1f, 1.0f);

    // barriles
    b2Body* barril1 = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 6.0f, 10.0f, 1.0f, 0.5f, 0.1f);
    barril1->SetTransform(b2Vec2(30, 50), 0.0f);

    b2Body* barril2 = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 6.0f, 10.0f, 1.0f, 0.5f, 0.1f);
    barril2->SetTransform(b2Vec2(50, 50), 0.0f);
}

void Game::CreatePj(sf::Vector2i mousePos)
{
    // Crear el cuerpo del maniquí
    b2Body* controlBody = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 2.0f, 4.0f, 1.0f, 0.5f, 0.1f);
    controlBody->SetTransform(b2Vec2(10, 80), 0.0f);

    // Cabeza
    b2Body* cabeza = Box2DHelper::CreateCircularDynamicBody(phyWorld, 1.0f, 1.0f, 0.5f, 0.1f);
    cabeza->SetTransform(b2Vec2(10, 83), 0.0f);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, controlBody->GetWorldCenter(), cabeza,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    // Brazos
    b2Body* brazoIzq = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 4.0f, 2.0f, 1.0f, 0.5f, 0.1f);
    brazoIzq->SetTransform(b2Vec2(10 - 3.0f, 80), 0.0f);

    b2Body* brazoDer = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 4.0f, 2.0f, 1.0f, 0.5f, 0.1f);
    brazoDer->SetTransform(b2Vec2(10 + 3.0f, 80), 0.0f);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, b2Vec2(10, 80), brazoIzq,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, b2Vec2(10, 80), brazoDer,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    // Piernas
    b2Body* piernaIzq = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 2.0f, 4.0f, 1.0f, 0.5f, 0.1f);
    piernaIzq->SetTransform(b2Vec2(10 - 1.0f, 80 - 5.0f), 0.0f);

    b2Body* piernaDer = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 2.0f, 4.0f, 1.0f, 0.5f, 0.1f);
    piernaDer->SetTransform(b2Vec2(10 + 1.0f, 80 - 5.0f), 0.0f);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, b2Vec2(10 - 2.0f, 80), piernaIzq,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, b2Vec2(10 + 2.0f, 80), piernaDer,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    // Dar fuerza
    sf::Vector2f worldMousePos = wnd->mapPixelToCoords(mousePos);
    b2Vec2 target(worldMousePos.x, worldMousePos.y);

    b2Vec2 direction = target - controlBody->GetPosition();
    direction.Normalize();

    float forceMagnitude = 200000; 
    b2Vec2 force = forceMagnitude * direction;
    controlBody->ApplyForceToCenter(force, true);
}