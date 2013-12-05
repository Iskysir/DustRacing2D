// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "resolutionmenu.hpp"
#include "confirmationmenu.hpp"
#include "game.hpp"
#include "settings.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include <MCLogger>

#include <QApplication>
#include <QDesktopWidget>
#include <QObject> // for tr()

#include <sstream>

struct Resolution
{
    int hRes, vRes;
};

static Resolution RESOLUTIONS[] =
{
    {400,  300},
    {640,  480},
    {800,  600},
    {854,  480},
    {960,  540},
    {1024, 768},
    {1280, 720},
    {1280, 960},
    {1366, 768},
    {1600, 900},
    {1920, 1080}
};

class ResolutionItem : public MTFH::MenuItem
{
public:

    class SaveResolutionAction : public MTFH::MenuItemAction
    {
    public:

        //! Constructor.
        SaveResolutionAction(ResolutionItem & parent, bool fullScreen)
        : m_parent(parent)
        , m_fullScreen(fullScreen)
        {
        }

        //! \reimp
        virtual void fire()
        {
            const int  hRes = m_parent.hRes();
            const int  vRes = m_parent.vRes();
            const bool nativeResolution = !hRes && !vRes;
            Settings::instance().saveResolution(hRes, vRes, nativeResolution, m_fullScreen);
            MCLogger().info()
                << "Resolution set: " << hRes << " " << vRes
                << " Native: " << nativeResolution
                << " Full screen: " << m_fullScreen;
            Game::instance().exitGame();
        }

    private:

        ResolutionItem & m_parent;
        bool m_fullScreen;
    };

    //! Constructor.
    ResolutionItem(
        ConfirmationMenu & confirmationMenu,
        int hRes, int vRes, bool fullScreen, int width, int height, std::wstring text = L"")
    : MenuItem(width, height, text)
    , m_confirmationMenu(confirmationMenu)
    , m_saveResolutionAction(new SaveResolutionAction(*this, fullScreen))
    , m_hRes(hRes)
    , m_vRes(vRes)
    {
    }

    //! Destructor.
    virtual ~ResolutionItem() {}

    //! \reimp
    virtual void setSelected(bool flag)
    {
        MenuItem::setSelected(flag);
        MTFH::MenuManager::instance().pushMenu(m_confirmationMenu.id());
        m_confirmationMenu.setText(QObject::tr("Restart to change the resolution.").toStdWString());
        m_confirmationMenu.setAcceptAction(m_saveResolutionAction);
        m_confirmationMenu.setCurrentIndex(1);
    }

    int hRes() const
    {
        return m_hRes;
    }

    int vRes() const
    {
        return m_vRes;
    }

private:

    ConfirmationMenu &      m_confirmationMenu;
    MTFH::MenuItemActionPtr m_saveResolutionAction;
    int                     m_hRes;
    int                     m_vRes;
};

ResolutionMenu::ResolutionMenu(
    ConfirmationMenu & confirmationMenu, std::string id, int width, int height, bool fullScreen)
: SurfaceMenu("settingsBack", id, width, height)
, m_confirmationMenu(confirmationMenu)
{
    const int resolutions = sizeof(RESOLUTIONS) / sizeof(Resolution);

    int availableResolutions = 0;
    for (int i = 0; i < resolutions; i++)
    {
        const int hRes = RESOLUTIONS[i].hRes;
        const int vRes = RESOLUTIONS[i].vRes;

        if (hRes <= QApplication::desktop()->width() && vRes <= QApplication::desktop()->height())
        {
            availableResolutions++;
        }
    }

    const int itemHeight = height / (availableResolutions + 2);

    for (int i = 0; i < resolutions; i++)
    {
        const int hRes = RESOLUTIONS[i].hRes;
        const int vRes = RESOLUTIONS[i].vRes;

        if (hRes < QApplication::desktop()->width() && vRes < QApplication::desktop()->height())
        {
            std::wstringstream resString;
            resString << hRes << "x" << vRes;
            ResolutionItem * resolution =
                new ResolutionItem(m_confirmationMenu, hRes, vRes, fullScreen, width, itemHeight, resString.str());
            resolution->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *resolution)));
            addItem(MTFH::MenuItemPtr(resolution));
        }
    }

    if (fullScreen)
    {
        ResolutionItem * resolution =
            new ResolutionItem(m_confirmationMenu, 0, 0, true, width, itemHeight,
                QObject::tr("Native resolution").toStdWString());
        resolution->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *resolution)));
        addItem(MTFH::MenuItemPtr(resolution));
    }

    enter();
}

void ResolutionMenu::enter()
{
    // Set the current active resolution selected
    for (unsigned int i = 0; i < itemCount(); i++)
    {
        if (auto resolution = dynamic_cast<ResolutionItem *>(item(i).get()))
        {
            int  hRes       = 0;
            int  vRes       = 0;
            bool native     = false;
            bool fullScreen = false;

            Settings::instance().loadResolution(hRes, vRes, native, fullScreen);
            if (resolution->hRes() == hRes && resolution->vRes() == vRes)
            {
                setCurrentIndex(resolution->index());
                break;
            }
        }
    }
}
