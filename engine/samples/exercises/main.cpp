/// [Include Components]
#include "components.hpp"
/// [Include Components]

/// [Include]
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/cubos.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/renderer/point_light.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/core/data/old/debug_serializer.hpp>

using namespace cubos::engine;
/// [Include]

/// [Include Entity Stuff]
using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::data::old::Debug;
/// [Include Entity Stuff]

static const Asset<VoxelGrid> CastleAsset = AnyAsset("6f42ae5a-59d1-5df3-8720-83b8df6dd536");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("4b0b2d65-685a-4e3a-8e39-a1fd2aff8cee");
static const Asset<InputBindings> BindingsAsset = AnyAsset("2bda251c-e8c5-48a7-8e3d-591f5a3a6c72");
static const Asset<Scene> SceneAsset = AnyAsset("077cb317-edcb-491e-a8df-a7ce0f02b573");

/// [Resource Decl]
struct Pop
{
    int count;
};
/// [Resource Decl]

/// [Entity Print]

static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

static void init(Read<Assets> assets, Write<Input> input)
{
    auto bindings = assets->read<InputBindings>(BindingsAsset);
    input->bind(*bindings);
    CUBOS_INFO("Loaded bindings: {}", Debug(input->bindings().at(0)));
}

static void spawnScene(Commands commands, Read<Assets> assets)
{
    auto sceneRead = assets->read(SceneAsset);
    commands.spawn(sceneRead->blueprint);
}

/*
static void checkEntitiesSystem(Query<Read<Num>> query, Read<Pop> pop)
{
    for (auto [entity, num] : query)
    {
        CUBOS_INFO("Entity '{}' of '{}'", num->value, pop->count);
    }
}
*/

static void setPaletteSystem(Read<Assets> assets, Write<Renderer> renderer)
{
    // Create a simple palette with 3 materials (red, green and blue).
    auto palette = assets->read(PaletteAsset);
    (*renderer)->setPalette(*palette);
}

/*
static void spawnVoxelGridSystem(Commands commands, Write<Assets> assets)
{
    // Create a 2x2x2 grid whose voxels alternate between the materials defined in the palette.
    auto gridAsset = assets->create(VoxelGrid{{1, 1, 1}, {1, 2, 3, 1, 2, 3, 1, 2}});

    // Spawn an entity with a renderable grid component and a identity transform.
    //Local to World is set autommaticly for the scene no worries
    commands.create(RenderableGrid{gridAsset, {-1.0F, 0.0F, -1.0F}}, LocalToWorld{});
}
*/

/*
static void spawnCastleSystem(Commands cmds, Read<Assets> assets)
{
    // Calculate the necessary offset to center the model on (0, 0, 0).
    auto castle = assets->read(CastleAsset);
    glm::vec3 offset = glm::vec3(castle->size().x, 0.0F, castle->size().z) / -2.0F;

    // Create the car entity
    cmds.create().add(RenderableGrid{CastleAsset, offset}).add(LocalToWorld{});
}
*/

static void spawnLightSystem(Commands commands)
{
    // Spawn a point light.
    commands.create()
        .add(PointLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 1.0F, .range = 50.0F})
        .add(Position{{0.0F, 5.0F, 0.0F}});
}

static void setEnvironmentSystem(Write<RendererEnvironment> env)
{
    env->ambient = {0.2F, 0.2F, 0.2F};
    env->skyGradient[0] = {0.1F, 0.2F, 0.4F};
    env->skyGradient[1] = {0.6F, 0.6F, 0.8F};
}

static void spawnCamerasSystem(Commands commands, Write<ActiveCameras> camera)
{
    // Spawn the a camera entity for the first viewport.
    camera->entities[0] =
        commands.create()
            .add(Camera{.fovY = 120.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{-25.0, 10.0F, -0.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 0.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

    // Add two other viewports using the same camera, which splits the screen in three.
    camera->entities[1] = commands.create()
            .add(Camera{.fovY = 120.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{-20.0, 10.0F, -20.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

    camera->entities[2] = commands.create()
            .add(Camera{.fovY = 120.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{0.0, 30.0F, 0.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, -1.0F, 0.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
}


static void Castle_Jump(Query<Write<Position>, Write<RenderableGrid>> query, Read<Input> input){

    glm::vec3 gravity{0.0f, -1.0f, 0.0f};
    glm::vec3 jump{0.0f,2.0f, 0.0f };

    for (auto [entity, position, grid] : query) {
        position->vec += gravity;

        if (position->vec.y <= 0.0f) {
            position->vec.y = 0.0f; 
        }
    }

    if(input->pressed("next-showcase")){        
        for(auto[entity, position, grid]: query)
        {
            position->vec += jump;
        }
    }
}

/// [Engine]
int main()
{
    Cubos cubos{};
    /// [Engine]

    cubos.addPlugin(inputPlugin);
    cubos.addPlugin(assetsPlugin);
    cubos.addPlugin(voxelsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(scenePlugin);
    
    /// [Component Add]
    cubos.addComponent<Num>();
    /// [Component Add]
    /// [Resource Add]
    cubos.addResource<Pop>();
    /// [Resource Add]


    /// [Entity System]
    cubos.startupSystem(config).tagged("cubos.settings");
    cubos.startupSystem(init).tagged("cubos.assets");
    cubos.startupSystem(setPaletteSystem).tagged("cubos.assets").after("cubos.renderer.init");
    ///cubos.system(spawnCastleSystem).tagged("cubos.assets");
    //cubos.startupSystem(spawnVoxelGridSystem);
    cubos.startupSystem(spawnLightSystem);
    cubos.startupSystem(setEnvironmentSystem);
    cubos.startupSystem(spawnCamerasSystem);
    //cubos.startupSystem(spawnEntitiesSystem);
    cubos.startupSystem(spawnScene).tagged("cubos.assets");

    ///BOUNCY CASTLE :)
    cubos.system(Castle_Jump).after("cubos.input.update");

    ///cubos.system(checkEntitiesSystem);
    /// [Entity System]

    /// [Run]
    cubos.run();
}
/// [Run]
