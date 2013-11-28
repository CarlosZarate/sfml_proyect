#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#define TIMESTEP 1.0f/60.0f     //TIEMPO DE REFRESCO
#define VELITER 10              //NUMERO DE ITERACION POR TICK PARA CALCULAR LA VELOCIDAD
#define POSITER 10              //NUMERO DE ITERACIONES POR TICK PARA CALCULAR LA POSICION

#define PPM 64.0f               //PIXELS POR METRO
#define MPP (1.0f/PPM)          //METROS POR PIXEL

//Estructura con los datos para representar una caja
struct Caja
{
    Caja()
    {
        m_Size = sf::Vector2f(50.0f, 50.0f);
    }
    Caja(sf::Vector2f size)
    {
        m_Size = size;
    }

    sf::Vector2f m_Size;

	b2Body *m_pBody;  //Necesario para obtener los datos generados por Box2D para esta 'pieza'
};


int main()
{
    //Vector con las cajas a dibujar
    std::vector<Caja> m_vCajas;

    //Definir Mundo Box2D (El parametro es la gravedad)
    b2World world(b2Vec2(0.0f, 10.0f));

    //Definir Suelo Box2D
	b2BodyDef sueloBodyDef;
	sueloBodyDef.position.Set(400.0f*MPP, 550.0f*MPP);
	b2Body* sueloBody = world.CreateBody(&sueloBodyDef);
	b2PolygonShape sueloBox;
	sueloBox.SetAsBox(800.0f/2*MPP, 100.0f/2*MPP);
	sueloBody->CreateFixture(&sueloBox, 0.0f);

    // Crear ventana
    sf::RenderWindow window(sf::VideoMode(800, 600), "Prueba Box2D");
    window.setFramerateLimit(60); //Hacer que los tiempos de refresco casen con los tiempos de Box2D

    // Bucle del juego
    while (window.isOpen())
    {
        // Bucle de Eventos de ventana
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Cerrar la ventana cuando se pulsa en el boton de cerrar ventana
            if (event.type == sf::Event::Closed)
                window.close();

            // Crear una caja donde el usuario hace click izquierdo
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    int randSize = 50 + rand()%(100+1 - 50);
                    sf::Vector2f posRaton = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                    Caja nuevaCaja = Caja(sf::Vector2f(randSize, randSize));

                    b2PolygonShape polyShape;
                    b2FixtureDef fixtureDef;
                    b2BodyDef bodyDef;

                    bodyDef.type = b2_dynamicBody; //Definimos un cuerpo dinamico (afectado por la gravedad y resto de objetos box2d creados)
                    bodyDef.position.Set(posRaton.x * MPP, posRaton.y * MPP);
                    nuevaCaja.m_pBody = world.CreateBody(&bodyDef);

                    polyShape.SetAsBox(randSize/2 * MPP, randSize/2 * MPP);

                    fixtureDef.shape = &polyShape;
                    fixtureDef.friction = 0.2f;
                    fixtureDef.restitution	= 0.3f;
                    fixtureDef.density	= 0.7f;

                    nuevaCaja.m_pBody->CreateFixture(&fixtureDef);

                    m_vCajas.push_back(nuevaCaja);
                }
            }
        }

        // Limpiar ventana con el color negro
        window.clear(sf::Color::Black);

        //Actulizar mundo Box2D
        world.Step(TIMESTEP, VELITER, POSITER);

        // Dibujamos las cajas creadas por el usuario
        for (std::vector<Caja>::iterator it = m_vCajas.begin(); it != m_vCajas.end(); ++it)
        {
            sf::RectangleShape rectCaja((*it).m_Size);
            rectCaja.setOrigin((*it).m_Size.x/2, (*it).m_Size.y/2);
            rectCaja.setFillColor(sf::Color::Green);

            float angle = (*it).m_pBody->GetAngle();
            b2Vec2 pos = (*it).m_pBody->GetPosition();

            rectCaja.setPosition(pos.x * PPM, pos.y * PPM);
            rectCaja.setRotation((angle * 180) / M_PI);

            window.draw(rectCaja); //Dibujamos el elemento en el buffer
        }

        // Dibujamos un rectangulo que actuara de suelo mas adelante
        sf::RectangleShape rectSuelo(sf::Vector2f(800, 100));
        rectSuelo.setOrigin(800/2, 100/2);
        rectSuelo.setPosition(sf::Vector2f(sueloBody->GetPosition().x*PPM, sueloBody->GetPosition().y*PPM));
        rectSuelo.setFillColor(sf::Color::Red);
        window.draw(rectSuelo); //Dibujamos el elemento en el buffer

        // Mostrar el buffer en pantalla
        window.display();
    }

    return 0;
}
