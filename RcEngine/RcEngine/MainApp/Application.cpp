#include "MainApp/Application.h"
#include "MainApp/Window.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Camera.h"
#include "Core/Exception.h"
#include "Core/Context.h"
#include "Core/XMLDom.h"
#include "Core/IModule.h"
#include "Core/ModuleManager.h"
#include "Input/InputSystem.h"

namespace RcEngine {

	using namespace Core;


	Application::Application( void )
		: mIsRunning(false), mAppPaused(false)
	{
		mConfigFile = "Config.xml";
		Context::Initialize();
		ModuleManager::Initialize();

		Context::GetSingleton().SetApplication(this);
	}

	Application::~Application( void )
	{
		ModuleManager::Finalize();
		Context::Finalize();
	}

	void Application::RunGame()
	{
		mIsRunning = true;

		Initialize();
		LoadContent();

		// Reset Game Timer
		mTimer.Reset();

		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );
		while( msg.message != WM_QUIT)
		{   
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );

			}
			else
			{
				Tick();
			}
		}

		UnloadContent();
	}



	void Application::Tick()
	{
		if (mActice)
		{
			Context::GetSingleton().GetInputSystem().Update();
		}

		mTimer.Tick();

		if(!mAppPaused)
			Update(mTimer.GetDeltaTime());
		else
			::Sleep(50);


		Render();
		
	}

	void Application::LoadAllModules()
	{
		ModuleManager::GetSingleton().Load(MT_Render_OpengGL);
		ModuleManager::GetSingleton().GetMoudleByType(MT_Render_OpengGL)->Initialise();

		ModuleManager::GetSingleton().Load(MT_Input_OIS);
		ModuleManager::GetSingleton().GetMoudleByType(MT_Input_OIS)->Initialise();
	}

	void Application::UnloadAllModules()
	{
		ModuleManager::GetSingleton().UnloadAll();
	}


	void Application::Window_ApplicationActivated()
	{
		mActice = true;

		Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->SetActice(true);
		Core::Context::GetSingleton().GetInputSystem().Update();
	}

	void Application::Window_ApplicationDeactivated()
	{
		mActice = false;
		Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->SetActice(false);
	}

	void Application::Window_Suspend()
	{
		mTimer.Stop();
	}

	void Application::Window_Resume()
	{
		mTimer.Start();
	}

	void Application::Window_Paint()
	{

	}

	void Application::ReadConfiguration()
	{
		mSettings.Left = 100;
		mSettings.Top = 100;
		mSettings.Width = 640;
		mSettings.Height = 480;
		mSettings.Fullscreen = false;
		mSettings.SampleCount = 0;
		mSettings.SampleQuality = 0;
		mSettings.ColorFormat = Render::PF_R8G8B8A8;
		mSettings.DepthStencilFormat = Render::PF_Depth24Stencil8;

	}

	void Application::Create()
	{
		// todo move this to context
		ReadConfiguration();

		// Create main window
		mMainWindow = new Window("RcEngine", mSettings);
		mMainWindow->PaintEvent.bind(this, &Application::Window_Paint);
		mMainWindow->SuspendEvent.bind(this, &Application::Window_Suspend);
		mMainWindow->ResumeEvent.bind(this, &Application::Window_Resume);
		mMainWindow->ApplicationActivatedEvent.bind(this, &Application::Window_ApplicationActivated);
		mMainWindow->ApplicationDeactivatedEvent.bind(this, &Application::Window_ApplicationDeactivated);

		// load all modules
		LoadAllModules();

		// Create render window
		Context::GetSingleton().GetRenderDevice().CreateRenderWindow(mSettings);

		// Show main window
		mMainWindow->ShowWindow();
	}

	void Application::Release()
	{
		//UnloadAllModules();
	}

} // Namespace RcEngine