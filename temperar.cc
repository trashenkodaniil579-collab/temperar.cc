--[[
╔══════════════════════════════════════════╗
║         dalol.xyz UI Library v2.0        ║
║         Modern · Clean · Animated        ║
╚══════════════════════════════════════════╝
--]]

local lib = {}; lib.__index = lib

local TS  = game:GetService("TweenService")
local UIS = game:GetService("UserInputService")
local RS  = game:GetService("RunService")
local PL  = game:GetService("Players")
local lp  = PL.LocalPlayer

-- ════════════════════════════════════════
--  THEME
-- ════════════════════════════════════════
local C = {
    bg          = Color3.fromRGB(8,   6,  20),
    bg2         = Color3.fromRGB(12,  10, 28),
    bg3         = Color3.fromRGB(10,  8,  24),
    border      = Color3.fromRGB(45,  25, 90),
    border2     = Color3.fromRGB(35,  20, 70),
    accent      = Color3.fromRGB(123, 47, 255),
    accentDark  = Color3.fromRGB(74,  24, 192),
    accentLight = Color3.fromRGB(160, 96, 255),
    text        = Color3.fromRGB(180, 160, 220),
    textDim     = Color3.fromRGB(80,  60, 140),
    textFaint   = Color3.fromRGB(45,  30, 90),
    secTitle    = Color3.fromRGB(130, 80, 230),
    white       = Color3.fromRGB(255, 255, 255),
    red         = Color3.fromRGB(255, 95,  87),
    yellow      = Color3.fromRGB(254, 188, 46),
    green       = Color3.fromRGB(40,  200, 64),
    success     = Color3.fromRGB(80,  220, 120),
    error_      = Color3.fromRGB(255, 80,  80),
}

lib.flags     = {}
lib.callbacks = {}
lib.theme     = C

-- ════════════════════════════════════════
--  UTILS
-- ════════════════════════════════════════
local function tw(obj, props, t, style, dir)
    local info = TweenInfo.new(
        t     or 0.2,
        style or Enum.EasingStyle.Quart,
        dir   or Enum.EasingDirection.Out
    )
    local ok, tw2 = pcall(function() return TS:Create(obj, info, props) end)
    if ok and tw2 then tw2:Play() end
end

local function new(class, props, parent)
    local ok, obj = pcall(Instance.new, class)
    if not ok then return nil end
    for k, v in pairs(props or {}) do
        pcall(function() obj[k] = v end)
    end
    if parent then obj.Parent = parent end
    return obj
end

local function corner(r, p)
    return new("UICorner", {CornerRadius = UDim.new(0, r)}, p)
end

local function stroke(col, thick, p)
    return new("UIStroke", {
        Color             = col,
        Thickness         = thick or 1,
        ApplyStrokeMode   = Enum.ApplyStrokeMode.Border,
        Transparency      = 0,
    }, p)
end

local function grad(c0, c1, rot, p)
    local g = new("UIGradient", {Rotation = rot or 90}, p)
    if g then g.Color = ColorSequence.new(c0, c1) end
    return g
end

local function pad(t, b, l, r, p)
    return new("UIPadding", {
        PaddingTop    = UDim.new(0, t or 0),
        PaddingBottom = UDim.new(0, b or 0),
        PaddingLeft   = UDim.new(0, l or 0),
        PaddingRight  = UDim.new(0, r or 0),
    }, p)
end

local function listLayout(spacing, dir, p)
    return new("UIListLayout", {
        SortOrder       = Enum.SortOrder.LayoutOrder,
        Padding         = UDim.new(0, spacing or 0),
        FillDirection   = dir or Enum.FillDirection.Vertical,
    }, p)
end

local function shadow(parent, intensity)
    intensity = intensity or 0.6
    local s = new("ImageLabel", {
        Size               = UDim2.new(1, 30, 1, 30),
        Position           = UDim2.new(0, -15, 0, -15),
        BackgroundTransparency = 1,
        Image              = "rbxassetid://6014261993",
        ImageColor3        = C.accent,
        ImageTransparency  = intensity,
        ZIndex             = 0,
        ScaleType          = Enum.ScaleType.Slice,
        SliceCenter        = Rect.new(49, 49, 450, 450),
    }, parent)
    return s
end

-- ════════════════════════════════════════
--  DRAGGING
-- ════════════════════════════════════════
local function makeDraggable(handle, target)
    local drag, startMouse, startPos
    handle.InputBegan:Connect(function(i)
        if i.UserInputType ~= Enum.UserInputType.MouseButton1 then return end
        drag = true
        startMouse = i.Position
        startPos   = target.Position
    end)
    handle.InputEnded:Connect(function(i)
        if i.UserInputType == Enum.UserInputType.MouseButton1 then drag = false end
    end)
    UIS.InputChanged:Connect(function(i)
        if drag and i.UserInputType == Enum.UserInputType.MouseMovement then
            local d = i.Position - startMouse
            target.Position = UDim2.new(
                startPos.X.Scale, startPos.X.Offset + d.X,
                startPos.Y.Scale, startPos.Y.Offset + d.Y
            )
        end
    end)
end

-- ════════════════════════════════════════
--  NOTIFICATIONS
-- ════════════════════════════════════════
local _notifGui, _notifHolder

local function ensureNotif()
    if _notifGui and _notifGui.Parent then return end
    _notifGui = new("ScreenGui", {
        Name = "DalolNotifs",
        ResetOnSpawn = false,
        ZIndexBehavior = Enum.ZIndexBehavior.Sibling,
        DisplayOrder = 999,
    })
    pcall(function() _notifGui.Parent = game:GetService("CoreGui") end)
    if not _notifGui.Parent then _notifGui.Parent = lp:WaitForChild("PlayerGui") end

    _notifHolder = new("Frame", {
        Size = UDim2.new(0, 250, 1, 0),
        Position = UDim2.new(1, -264, 0, 0),
        BackgroundTransparency = 1,
        ZIndex = 999,
    }, _notifGui)
    local ll = listLayout(8, nil, _notifHolder)
    ll.VerticalAlignment = Enum.VerticalAlignment.Bottom
    ll.HorizontalAlignment = Enum.HorizontalAlignment.Center
    pad(0, 16, 0, 0, _notifHolder)
end

function lib:notify(title, msg, icon, duration)
    duration = duration or 4
    ensureNotif()

    local card = new("Frame", {
        Size = UDim2.new(1, 0, 0, 72),
        BackgroundColor3 = C.bg2,
        BorderSizePixel = 0,
        ClipsDescendants = true,
        BackgroundTransparency = 1,
    }, _notifHolder)
    corner(12, card)
    stroke(C.border, 1, card)

    -- accent left bar
    new("Frame", {
        Size = UDim2.new(0, 2, 1, 0),
        BackgroundColor3 = C.accent,
        BorderSizePixel = 0,
        ZIndex = 2,
    }, card)

    -- icon
    local icnFrame = new("Frame", {
        Size = UDim2.new(0, 28, 0, 28),
        Position = UDim2.new(0, 12, 0, 12),
        BackgroundColor3 = C.accentDark,
        BorderSizePixel = 0,
        ZIndex = 2,
    }, card)
    corner(8, icnFrame)
    grad(C.accentDark, C.accent, 135, icnFrame)
    new("TextLabel", {
        Size = UDim2.new(1,0,1,0),
        BackgroundTransparency = 1,
        Text = icon or "✦",
        TextColor3 = C.white,
        Font = Enum.Font.GothamBold,
        TextSize = 13,
        ZIndex = 3,
    }, icnFrame)

    -- title
    new("TextLabel", {
        Size = UDim2.new(1, -55, 0, 16),
        Position = UDim2.new(0, 48, 0, 11),
        BackgroundTransparency = 1,
        Text = title or "NOTICE",
        TextColor3 = C.text,
        Font = Enum.Font.GothamBold,
        TextSize = 11,
        TextXAlignment = Enum.TextXAlignment.Left,
        ZIndex = 3,
    }, card)

    -- message
    new("TextLabel", {
        Size = UDim2.new(1, -55, 0, 28),
        Position = UDim2.new(0, 48, 0, 28),
        BackgroundTransparency = 1,
        Text = msg or "",
        TextColor3 = C.textDim,
        Font = Enum.Font.Gotham,
        TextSize = 11,
        TextXAlignment = Enum.TextXAlignment.Left,
        TextWrapped = true,
        ZIndex = 3,
    }, card)

    -- progress bar
    local barBg = new("Frame", {
        Size = UDim2.new(1, -14, 0, 2),
        Position = UDim2.new(0, 7, 1, -5),
        BackgroundColor3 = C.border2,
        BorderSizePixel = 0,
        ZIndex = 3,
    }, card)
    corner(2, barBg)
    local barFill = new("Frame", {
        Size = UDim2.new(1, 0, 1, 0),
        BackgroundColor3 = C.accent,
        BorderSizePixel = 0,
        ZIndex = 4,
    }, barBg)
    corner(2, barFill)
    grad(C.accentDark, C.accentLight, 0, barFill)

    -- animate in
    card.Size = UDim2.new(1, 0, 0, 0)
    tw(card, {Size = UDim2.new(1, 0, 0, 72), BackgroundTransparency = 0}, 0.3, Enum.EasingStyle.Back)
    tw(barFill, {Size = UDim2.new(0, 0, 1, 0)}, duration, Enum.EasingStyle.Linear, Enum.EasingDirection.In)

    task.delay(duration, function()
        tw(card, {Size = UDim2.new(1, 0, 0, 0), BackgroundTransparency = 1}, 0.25)
        task.wait(0.3)
        pcall(function() card:Destroy() end)
    end)
end

-- ════════════════════════════════════════
--  WATERMARK
-- ════════════════════════════════════════
function lib:setWatermark(text)
    if self._wmlbl then self._wmlbl.Text = text end
end

local function buildWatermark(gui, text)
    local frame = new("Frame", {
        Size = UDim2.new(0, 0, 0, 26),
        Position = UDim2.new(0, 14, 0, 14),
        AutomaticSize = Enum.AutomaticSize.X,
        BackgroundColor3 = C.bg2,
        BorderSizePixel = 0,
        ZIndex = 100,
    }, gui)
    corner(7, frame)
    stroke(C.border, 1, frame)
    pad(0, 0, 10, 10, frame)

    local dot = new("Frame", {
        Size = UDim2.new(0, 6, 0, 6),
        Position = UDim2.new(0, 0, 0.5, -3),
        BackgroundColor3 = C.accent,
        BorderSizePixel = 0,
        ZIndex = 101,
    }, frame)
    corner(3, dot)

    -- pulse dot
    spawn(function()
        while frame and frame.Parent do
            tw(dot, {BackgroundTransparency = 0.6}, 1)
            task.wait(1)
            tw(dot, {BackgroundTransparency = 0}, 1)
            task.wait(1)
        end
    end)

    local lbl = new("TextLabel", {
        Size = UDim2.new(0, 0, 1, 0),
        Position = UDim2.new(0, 12, 0, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        BackgroundTransparency = 1,
        Text = text or "dalol.xyz",
        TextColor3 = C.accentLight,
        Font = Enum.Font.GothamBold,
        TextSize = 11,
        ZIndex = 101,
    }, frame)

    return lbl
end

-- ════════════════════════════════════════
--  CONFIG SYSTEM
-- ════════════════════════════════════════
local CFG_DIR = "dalol_configs/"
local HS = game:GetService("HttpService")

local function mkDir()
    if not isfolder(CFG_DIR) then makefolder(CFG_DIR) end
end

function lib:saveConfig(name)
    mkDir()
    local data = {}
    for k, v in pairs(self.flags) do
        if type(v) == "number" or type(v) == "boolean" or type(v) == "string" then
            data[k] = v
        elseif typeof(v) == "Color3" then
            data[k] = {_c3 = true, r = v.R, g = v.G, b = v.B}
        end
    end
    pcall(writefile, CFG_DIR .. name .. ".json", HS:JSONEncode(data))
    self:notify("CONFIG SAVED", name .. ".json", "💾", 3)
end

function lib:loadConfig(name)
    mkDir()
    local path = CFG_DIR .. name .. ".json"
    if not isfile(path) then self:notify("ERROR", "Config not found", "✕", 3) return end
    local ok, data = pcall(function() return HS:JSONDecode(readfile(path)) end)
    if not ok then self:notify("ERROR", "Parse failed", "✕", 3) return end
    for k, v in pairs(data) do
        if type(v) == "table" and v._c3 then
            self.flags[k] = Color3.new(v.r, v.g, v.b)
        else
            self.flags[k] = v
        end
        if self.callbacks[k] then pcall(self.callbacks[k], self.flags[k]) end
    end
    self:notify("CONFIG LOADED", name .. ".json", "✓", 3)
end

function lib:listConfigs()
    mkDir()
    local out = {}
    for _, f in ipairs(listfiles(CFG_DIR)) do
        table.insert(out, f:gsub(CFG_DIR,""):gsub("%.json$",""))
    end
    return out
end

function lib:deleteConfig(name)
    local path = CFG_DIR .. name .. ".json"
    if isfile(path) then pcall(delfile, path) end
end

-- ════════════════════════════════════════
--  WINDOW
-- ════════════════════════════════════════
function lib:window(opts)
    opts = opts or {}
    local W = opts.width  or 780
    local H = opts.height or 520
    local name = opts.name or "dalol.xyz"

    -- ScreenGui
    local gui = new("ScreenGui", {
        Name = "DalolUI",
        ResetOnSpawn = false,
        ZIndexBehavior = Enum.ZIndexBehavior.Sibling,
        DisplayOrder = 100,
    })
    pcall(function() gui.Parent = game:GetService("CoreGui") end)
    if not gui.Parent then gui.Parent = lp:WaitForChild("PlayerGui") end
    self._gui = gui

    -- Blur
    local blur = new("BlurEffect", {Size = 0}, game:GetService("Lighting"))
    tw(blur, {Size = 10}, 0.5)

    -- Window frame
    local win = new("Frame", {
        Name = "Window",
        Size = UDim2.new(0, W, 0, H),
        Position = UDim2.new(0.5, -W/2, 0.5, -H/2),
        BackgroundColor3 = C.bg,
        BorderSizePixel = 0,
        ClipsDescendants = false,
    }, gui)
    corner(14, win)
    stroke(C.border, 1, win)

    -- Clip inner content separately
    local clip = new("Frame", {
        Size = UDim2.new(1, 0, 1, 0),
        BackgroundTransparency = 1,
        ClipsDescendants = true,
        ZIndex = 1,
    }, win)

    -- Glow
    shadow(win, 0.55)

    -- Open animation
    win.BackgroundTransparency = 1
    win.Size = UDim2.new(0, W, 0, 10)
    tw(win, {Size = UDim2.new(0, W, 0, H), BackgroundTransparency = 0}, 0.4, Enum.EasingStyle.Back)

    -- ── TITLEBAR ──
    local tb = new("Frame", {
        Size = UDim2.new(1, 0, 0, 50),
        BackgroundColor3 = Color3.fromRGB(10, 8, 26),
        BorderSizePixel = 0,
        ZIndex = 10,
    }, clip)
    grad(Color3.fromRGB(14, 10, 38), Color3.fromRGB(8, 6, 20), 180, tb)

    -- top glow line on titlebar
    new("Frame", {
        Size = UDim2.new(0.6, 0, 0, 1),
        Position = UDim2.new(0.2, 0, 0, 0),
        BackgroundColor3 = C.accent,
        BorderSizePixel = 0,
        BackgroundTransparency = 0.5,
        ZIndex = 11,
    }, tb)

    -- bottom border
    new("Frame", {
        Size = UDim2.new(1, 0, 0, 1),
        Position = UDim2.new(0, 0, 1, -1),
        BackgroundColor3 = C.border,
        BorderSizePixel = 0,
        ZIndex = 11,
    }, tb)

    -- Logo
    local logo = new("Frame", {
        Size = UDim2.new(0, 30, 0, 30),
        Position = UDim2.new(0, 14, 0.5, -15),
        BackgroundColor3 = C.accentDark,
        BorderSizePixel = 0,
        ZIndex = 11,
    }, tb)
    corner(9, logo)
    grad(C.accentDark, C.accentLight, 135, logo)
    new("TextLabel", {
        Size = UDim2.new(1,0,1,0),
        BackgroundTransparency = 1,
        Text = "✦",
        TextColor3 = C.white,
        Font = Enum.Font.GothamBold,
        TextSize = 15,
        ZIndex = 12,
    }, logo)

    -- Name
    new("TextLabel", {
        Size = UDim2.new(0, 200, 0, 18),
        Position = UDim2.new(0, 54, 0, 10),
        BackgroundTransparency = 1,
        Text = name:upper(),
        TextColor3 = Color3.fromRGB(210, 190, 255),
        Font = Enum.Font.GothamBold,
        TextSize = 14,
        TextXAlignment = Enum.TextXAlignment.Left,
        ZIndex = 11,
    }, tb)

    new("TextLabel", {
        Size = UDim2.new(0, 200, 0, 14),
        Position = UDim2.new(0, 54, 0, 29),
        BackgroundTransparency = 1,
        Text = os.date("%b %d %Y") .. "  ·  v2.0",
        TextColor3 = C.textFaint,
        Font = Enum.Font.Gotham,
        TextSize = 10,
        TextXAlignment = Enum.TextXAlignment.Left,
        ZIndex = 11,
    }, tb)

    -- Window control buttons
    local btnColors = {C.green, C.yellow, C.red}
    for i, col in ipairs(btnColors) do
        local b = new("Frame", {
            Size = UDim2.new(0, 12, 0, 12),
            Position = UDim2.new(0, W - 20 - (i-1)*18, 0.5, -6),
            BackgroundColor3 = col,
            BorderSizePixel = 0,
            ZIndex = 12,
        }, tb)
        corner(6, b)
        new("ImageLabel", {
            Size = UDim2.new(1,8,1,8),
            Position = UDim2.new(0,-4,0,-4),
            BackgroundTransparency = 1,
            Image = "rbxassetid://6014261993",
            ImageColor3 = col,
            ImageTransparency = 0.6,
            ZIndex = 11,
            ScaleType = Enum.ScaleType.Slice,
            SliceCenter = Rect.new(49,49,450,450),
        }, b)
    end

    -- Close button
    local closeHit = new("TextButton", {
        Size = UDim2.new(0, 12, 0, 12),
        Position = UDim2.new(0, W - 20, 0.5, -6),
        BackgroundTransparency = 1,
        Text = "",
        ZIndex = 13,
    }, tb)
    closeHit.MouseButton1Click:Connect(function()
        tw(win, {Size = UDim2.new(0, W, 0, 0), BackgroundTransparency = 1}, 0.3, Enum.EasingStyle.Quart)
        tw(blur, {Size = 0}, 0.4)
        task.wait(0.35)
        pcall(function() gui:Destroy() end)
    end)

    makeDraggable(tb, win)

    -- ── TABS BAR ──
    local tabBar = new("Frame", {
        Size = UDim2.new(1, 0, 0, 38),
        Position = UDim2.new(0, 0, 0, 50),
        BackgroundColor3 = Color3.fromRGB(6, 4, 18),
        BorderSizePixel = 0,
        ZIndex = 9,
    }, clip)

    new("Frame", {
        Size = UDim2.new(1, 0, 0, 1),
        Position = UDim2.new(0, 0, 1, -1),
        BackgroundColor3 = C.border2,
        BorderSizePixel = 0,
        ZIndex = 10,
    }, tabBar)

    local tabList = listLayout(0, Enum.FillDirection.Horizontal, tabBar)
    tabList.VerticalAlignment = Enum.VerticalAlignment.Center
    pad(0, 2, 10, 0, tabBar)

    -- ── CONTENT AREA ──
    local contentArea = new("Frame", {
        Size = UDim2.new(1, 0, 1, -88),
        Position = UDim2.new(0, 0, 0, 88),
        BackgroundTransparency = 1,
        ClipsDescendants = true,
        ZIndex = 5,
    }, clip)

    -- ── WATERMARK ──
    if opts.watermark ~= false then
        self._wmlbl = buildWatermark(gui, name)
    end

    -- ────────────────────────────────────
    --  TAB MANAGEMENT
    -- ────────────────────────────────────
    local tabs       = {}
    local activeTab  = nil

    local winObj = {}

    function winObj:tab(tabOpts)
        tabOpts = tabOpts or {}
        local tabName = tabOpts.name or ("Tab" .. #tabs+1)

        -- tab button
        local tabBtn = new("TextButton", {
            Size = UDim2.new(0, 0, 1, 0),
            AutomaticSize = Enum.AutomaticSize.X,
            BackgroundTransparency = 1,
            Text = "",
            ZIndex = 10,
            LayoutOrder = #tabs + 1,
        }, tabBar)
        pad(0, 2, 14, 14, tabBtn)

        local tabLbl = new("TextLabel", {
            Size = UDim2.new(1, 0, 1, 0),
            BackgroundTransparency = 1,
            Text = tabName:upper(),
            TextColor3 = C.textFaint,
            Font = Enum.Font.GothamBold,
            TextSize = 10,
            ZIndex = 11,
        }, tabBtn)

        local tabLine = new("Frame", {
            Size = UDim2.new(0, 0, 0, 2),
            Position = UDim2.new(0.5, 0, 1, 0),
            BackgroundColor3 = C.accent,
            BorderSizePixel = 0,
            ZIndex = 11,
        }, tabBtn)
        corner(1, tabLine)

        local tabDot = new("Frame", {
            Size = UDim2.new(0, 4, 0, 4),
            Position = UDim2.new(0.5, -2, 1, -1),
            BackgroundColor3 = C.accent,
            BorderSizePixel = 0,
            BackgroundTransparency = 1,
            ZIndex = 12,
        }, tabBtn)
        corner(2, tabDot)

        -- content page
        local page = new("Frame", {
            Size = UDim2.new(1, 0, 1, 0),
            BackgroundTransparency = 1,
            Visible = false,
            ZIndex = 5,
        }, contentArea)

        local colHolder = new("Frame", {
            Size = UDim2.new(1, -24, 1, -24),
            Position = UDim2.new(0, 12, 0, 12),
            BackgroundTransparency = 1,
        }, page)
        listLayout(10, Enum.FillDirection.Horizontal, colHolder)

        local entry = {btn=tabBtn, lbl=tabLbl, line=tabLine, dot=tabDot, page=page, cols=colHolder}
        table.insert(tabs, entry)

        local function activate()
            for _, t in ipairs(tabs) do
                tw(t.lbl,  {TextColor3 = C.textFaint}, 0.2)
                tw(t.line, {Size = UDim2.new(0, 0, 0, 2)}, 0.2)
                tw(t.dot,  {BackgroundTransparency = 1}, 0.15)
                t.page.Visible = false
            end
            tw(tabLbl,  {TextColor3 = C.accentLight}, 0.2)
            tw(tabLine, {Size = UDim2.new(1, 0, 0, 2)}, 0.25, Enum.EasingStyle.Back)
            tw(tabDot,  {BackgroundTransparency = 0}, 0.2)
            page.Position = UDim2.new(0.04, 0, 0, 0)
            page.BackgroundTransparency = 1
            page.Visible = true
            tw(page, {Position = UDim2.new(0, 0, 0, 0), BackgroundTransparency = 0}, 0.2, Enum.EasingStyle.Quart)
            activeTab = entry
        end

        tabBtn.MouseButton1Click:Connect(activate)
        if #tabs == 1 then task.defer(activate) end

        -- ── TAB OBJECT ──
        local tabObj = {}

        function tabObj:column(size)
            local colSize = size or 0.5
            local col = new("Frame", {
                Size = UDim2.new(colSize, -(#tabs > 0 and 5 or 0), 1, 0),
                BackgroundTransparency = 1,
                LayoutOrder = 1,
                AutomaticSize = Enum.AutomaticSize.Y,
            }, colHolder)

            local scroll = new("ScrollingFrame", {
                Size = UDim2.new(1, 0, 1, 0),
                BackgroundTransparency = 1,
                BorderSizePixel = 0,
                ScrollBarThickness = 2,
                ScrollBarImageColor3 = C.accent,
                ScrollBarImageTransparency = 0.5,
                CanvasSize = UDim2.new(0, 0, 0, 0),
                AutomaticCanvasSize = Enum.AutomaticSize.Y,
            }, col)

            local inner = new("Frame", {
                Size = UDim2.new(1, 0, 0, 0),
                BackgroundTransparency = 1,
                AutomaticSize = Enum.AutomaticSize.Y,
            }, scroll)
            listLayout(8, nil, inner)

            local colObj = {}

            -- ──────────────────────────────
            --  SECTION
            -- ──────────────────────────────
            function colObj:section(sOpts)
                sOpts = sOpts or {}
                local sName = sOpts.name or "Section"

                local sec = new("Frame", {
                    Size = UDim2.new(1, 0, 0, 0),
                    BackgroundColor3 = C.bg2,
                    BorderSizePixel = 0,
                    AutomaticSize = Enum.AutomaticSize.Y,
                    ClipsDescendants = false,
                }, inner)
                corner(12, sec)
                stroke(C.border2, 1, sec)

                -- header
                local hdr = new("Frame", {
                    Size = UDim2.new(1, 0, 0, 32),
                    BackgroundColor3 = C.bg3,
                    BorderSizePixel = 0,
                    ZIndex = 2,
                }, sec)
                corner(12, hdr)

                -- fix bottom corners of header
                new("Frame", {
                    Size = UDim2.new(1, 0, 0, 12),
                    Position = UDim2.new(0, 0, 1, -12),
                    BackgroundColor3 = C.bg3,
                    BorderSizePixel = 0,
                    ZIndex = 2,
                }, hdr)

                new("Frame", {
                    Size = UDim2.new(1, 0, 0, 1),
                    Position = UDim2.new(0, 0, 1, -1),
                    BackgroundColor3 = C.border2,
                    BorderSizePixel = 0,
                    ZIndex = 3,
                }, hdr)

                -- accent bar
                local abar = new("Frame", {
                    Size = UDim2.new(0, 2, 0, 13),
                    Position = UDim2.new(0, 10, 0.5, -6),
                    BackgroundColor3 = C.accent,
                    BorderSizePixel = 0,
                    ZIndex = 3,
                }, hdr)
                corner(2, abar)

                new("TextLabel", {
                    Size = UDim2.new(1, -24, 1, 0),
                    Position = UDim2.new(0, 20, 0, 0),
                    BackgroundTransparency = 1,
                    Text = sName:upper(),
                    TextColor3 = C.secTitle,
                    Font = Enum.Font.GothamBold,
                    TextSize = 10,
                    TextXAlignment = Enum.TextXAlignment.Left,
                    ZIndex = 3,
                }, hdr)

                -- body
                local body = new("Frame", {
                    Size = UDim2.new(1, 0, 0, 0),
                    Position = UDim2.new(0, 0, 0, 32),
                    BackgroundTransparency = 1,
                    AutomaticSize = Enum.AutomaticSize.Y,
                    ZIndex = 2,
                }, sec)
                pad(4, 10, 11, 11, body)
                listLayout(2, nil, body)

                -- ── SECTION ELEMENTS ──
                local secObj = {}

                local function mkRow(h)
                    return new("Frame", {
                        Size = UDim2.new(1, 0, 0, h or 30),
                        BackgroundTransparency = 1,
                        ZIndex = 3,
                    }, body)
                end

                local function mkLbl(parent, text, col2, font, size)
                    return new("TextLabel", {
                        Size = UDim2.new(0.55, 0, 1, 0),
                        BackgroundTransparency = 1,
                        Text = text,
                        TextColor3 = col2 or C.text,
                        Font = font or Enum.Font.Gotham,
                        TextSize = size or 12,
                        TextXAlignment = Enum.TextXAlignment.Left,
                        ZIndex = 4,
                    }, parent)
                end

                -- ── TOGGLE ──
                function secObj:toggle(opts)
                    opts = opts or {}
                    local flag = opts.flag or opts.name
                    local cb   = opts.callback
                    lib.flags[flag] = opts.default == true

                    local row = mkRow(30)
                    mkLbl(row, opts.name or "Toggle")

                    local track = new("Frame", {
                        Size = UDim2.new(0, 36, 0, 18),
                        Position = UDim2.new(1, -36, 0.5, -9),
                        BackgroundColor3 = Color3.fromRGB(20, 15, 45),
                        BorderSizePixel = 0,
                        ZIndex = 4,
                    }, row)
                    corner(9, track)
                    stroke(C.border, 1, track)

                    local knob = new("Frame", {
                        Size = UDim2.new(0, 12, 0, 12),
                        Position = UDim2.new(0, 2, 0.5, -6),
                        BackgroundColor3 = Color3.fromRGB(80, 60, 140),
                        BorderSizePixel = 0,
                        ZIndex = 5,
                    }, track)
                    corner(6, knob)

                    local on = opts.default == true

                    local function set(v, anim)
                        on = v
                        lib.flags[flag] = v
                        if v then
                            if anim then
                                tw(track, {BackgroundColor3 = C.accentDark}, 0.2)
                                tw(knob, {
                                    Position = UDim2.new(0, 20, 0.5, -6),
                                    BackgroundColor3 = C.white,
                                    Size = UDim2.new(0, 12, 0, 12),
                                }, 0.2, Enum.EasingStyle.Back)
                            else
                                track.BackgroundColor3 = C.accentDark
                                knob.Position = UDim2.new(0, 20, 0.5, -6)
                                knob.BackgroundColor3 = C.white
                            end
                        else
                            if anim then
                                tw(track, {BackgroundColor3 = Color3.fromRGB(20, 15, 45)}, 0.2)
                                tw(knob, {
                                    Position = UDim2.new(0, 2, 0.5, -6),
                                    BackgroundColor3 = Color3.fromRGB(80, 60, 140),
                                }, 0.2, Enum.EasingStyle.Back)
                            else
                                track.BackgroundColor3 = Color3.fromRGB(20, 15, 45)
                                knob.Position = UDim2.new(0, 2, 0.5, -6)
                                knob.BackgroundColor3 = Color3.fromRGB(80, 60, 140)
                            end
                        end
                        if cb then pcall(cb, v) end
                    end

                    set(on, false)
                    lib.callbacks[flag] = function(v) set(v, false) end

                    local btn = new("TextButton", {
                        Size = UDim2.new(1, 0, 1, 0),
                        BackgroundTransparency = 1,
                        Text = "",
                        ZIndex = 6,
                    }, row)
                    btn.MouseButton1Click:Connect(function()
                        -- squeeze animation
                        tw(knob, {Size = UDim2.new(0, 8, 0, 12)}, 0.07)
                        task.delay(0.07, function()
                            tw(knob, {Size = UDim2.new(0, 12, 0, 12)}, 0.15, Enum.EasingStyle.Back)
                        end)
                        set(not on, true)
                    end)

                    -- hover
                    btn.MouseEnter:Connect(function()
                        tw(track, {BackgroundTransparency = 0}, 0.1)
                    end)

                    local togObj = {}
                    function togObj:colorpicker(cpOpts) secObj:colorpicker(cpOpts) return togObj end
                    function togObj:getState() return on end
                    function togObj:setState(v) set(v, true) end
                    return togObj
                end

                -- ── SLIDER ──
                function secObj:slider(opts)
                    opts = opts or {}
                    local flag    = opts.flag or opts.name
                    local min     = opts.min  or 0
                    local max     = opts.max  or 100
                    local def     = opts.default or min
                    local suffix  = opts.suffix or ""
                    local interval = opts.interval or 1
                    local cb      = opts.callback

                    lib.flags[flag] = def

                    local wrap = new("Frame", {
                        Size = UDim2.new(1, 0, 0, 38),
                        BackgroundTransparency = 1,
                        ZIndex = 3,
                    }, body)

                    local topRow = new("Frame", {
                        Size = UDim2.new(1, 0, 0, 18),
                        BackgroundTransparency = 1,
                        ZIndex = 4,
                    }, wrap)

                    new("TextLabel", {
                        Size = UDim2.new(0.7, 0, 1, 0),
                        BackgroundTransparency = 1,
                        Text = opts.name or "Slider",
                        TextColor3 = C.text,
                        Font = Enum.Font.Gotham,
                        TextSize = 12,
                        TextXAlignment = Enum.TextXAlignment.Left,
                        ZIndex = 5,
                    }, topRow)

                    local valBg = new("Frame", {
                        Size = UDim2.new(0, 0, 0, 16),
                        Position = UDim2.new(1, 0, 0.5, -8),
                        AutomaticSize = Enum.AutomaticSize.X,
                        BackgroundColor3 = Color3.fromRGB(20, 15, 45),
                        BorderSizePixel = 0,
                        ZIndex = 5,
                    }, topRow)
                    corner(4, valBg)
                    stroke(C.border, 1, valBg)
                    pad(0, 0, 6, 6, valBg)

                    local valLbl = new("TextLabel", {
                        Size = UDim2.new(0, 0, 1, 0),
                        AutomaticSize = Enum.AutomaticSize.X,
                        BackgroundTransparency = 1,
                        Text = tostring(def) .. suffix,
                        TextColor3 = C.accentLight,
                        Font = Enum.Font.GothamBold,
                        TextSize = 10,
                        ZIndex = 6,
                    }, valBg)

                    local track = new("Frame", {
                        Size = UDim2.new(1, 0, 0, 3),
                        Position = UDim2.new(0, 0, 0, 26),
                        BackgroundColor3 = Color3.fromRGB(20, 15, 45),
                        BorderSizePixel = 0,
                        ZIndex = 4,
                    }, wrap)
                    corner(3, track)

                    local fill = new("Frame", {
                        Size = UDim2.new((def-min)/(max-min), 0, 1, 0),
                        BackgroundColor3 = C.accent,
                        BorderSizePixel = 0,
                        ZIndex = 5,
                    }, track)
                    corner(3, fill)
                    grad(C.accentDark, C.accentLight, 0, fill)

                    local knob = new("Frame", {
                        Size = UDim2.new(0, 13, 0, 13),
                        Position = UDim2.new((def-min)/(max-min), -6, 0.5, -6),
                        BackgroundColor3 = C.white,
                        BorderSizePixel = 0,
                        ZIndex = 6,
                    }, track)
                    corner(7, knob)
                    stroke(C.accent, 2, knob)

                    local function setValue(pct)
                        pct = math.clamp(pct, 0, 1)
                        local raw = min + (max - min) * pct
                        local v   = math.floor(raw / interval + 0.5) * interval
                        v         = math.clamp(v, min, max)
                        lib.flags[flag] = v
                        local p2 = (v - min) / (max - min)
                        fill.Size     = UDim2.new(p2, 0, 1, 0)
                        knob.Position = UDim2.new(p2, -6, 0.5, -6)
                        valLbl.Text   = tostring(v) .. suffix
                        if cb then pcall(cb, v) end
                    end

                    lib.callbacks[flag] = function(v) setValue((v-min)/(max-min)) end

                    local sliding = false
                    track.InputBegan:Connect(function(i)
                        if i.UserInputType == Enum.UserInputType.MouseButton1 then
                            sliding = true
                            tw(knob, {Size = UDim2.new(0, 15, 0, 15), Position = UDim2.new(knob.Position.X.Scale, -7, 0.5, -7)}, 0.1, Enum.EasingStyle.Back)
                        end
                    end)
                    UIS.InputEnded:Connect(function(i)
                        if i.UserInputType == Enum.UserInputType.MouseButton1 and sliding then
                            sliding = false
                            tw(knob, {Size = UDim2.new(0, 13, 0, 13)}, 0.1)
                        end
                    end)
                    UIS.InputChanged:Connect(function(i)
                        if sliding and i.UserInputType == Enum.UserInputType.MouseMovement then
                            local pct = (i.Position.X - track.AbsolutePosition.X) / track.AbsoluteSize.X
                            setValue(pct)
                        end
                    end)
                end

                -- ── DROPDOWN ──
                function secObj:dropdown(opts)
                    opts = opts or {}
                    local flag = opts.flag or opts.name
                    local list_ = opts.list or {}
                    local def  = opts.default or list_[1] or ""
                    local cb   = opts.callback

                    lib.flags[flag] = def

                    local container = new("Frame", {
                        Size = UDim2.new(1, 0, 0, 0),
                        BackgroundTransparency = 1,
                        AutomaticSize = Enum.AutomaticSize.Y,
                        ZIndex = 20,
                    }, body)
                    listLayout(3, nil, container)

                    local row = new("Frame", {
                        Size = UDim2.new(1, 0, 0, 30),
                        BackgroundTransparency = 1,
                        ZIndex = 20,
                    }, container)
                    mkLbl(row, opts.name or "Dropdown")

                    local selFrame = new("Frame", {
                        Size = UDim2.new(0.44, 0, 0, 24),
                        Position = UDim2.new(0.56, 0, 0.5, -12),
                        BackgroundColor3 = C.bg3,
                        BorderSizePixel = 0,
                        ZIndex = 21,
                    }, row)
                    corner(7, selFrame)
                    stroke(C.border, 1, selFrame)

                    local selLbl = new("TextLabel", {
                        Size = UDim2.new(1, -20, 1, 0),
                        Position = UDim2.new(0, 8, 0, 0),
                        BackgroundTransparency = 1,
                        Text = def,
                        TextColor3 = C.text,
                        Font = Enum.Font.Gotham,
                        TextSize = 11,
                        TextXAlignment = Enum.TextXAlignment.Left,
                        ZIndex = 22,
                    }, selFrame)

                    local arrow = new("TextLabel", {
                        Size = UDim2.new(0, 16, 1, 0),
                        Position = UDim2.new(1, -18, 0, 0),
                        BackgroundTransparency = 1,
                        Text = "▾",
                        TextColor3 = C.accent,
                        Font = Enum.Font.GothamBold,
                        TextSize = 11,
                        ZIndex = 22,
                    }, selFrame)

                    -- options
                    local optFrame = new("Frame", {
                        Size = UDim2.new(1, 0, 0, 0),
                        BackgroundColor3 = C.bg3,
                        BorderSizePixel = 0,
                        ClipsDescendants = true,
                        ZIndex = 25,
                    }, container)
                    corner(8, optFrame)
                    stroke(C.border, 1, optFrame)
                    listLayout(0, nil, optFrame)

                    local totalH = 0
                    local isOpen = false

                    for _, item in ipairs(list_) do
                        local h = 28
                        totalH = totalH + h

                        local optBtn = new("TextButton", {
                            Size = UDim2.new(1, 0, 0, h),
                            BackgroundColor3 = C.bg3,
                            BorderSizePixel = 0,
                            Text = "",
                            ZIndex = 26,
                        }, optFrame)

                        local dot = new("Frame", {
                            Size = UDim2.new(0, 4, 0, 4),
                            Position = UDim2.new(0, 10, 0.5, -2),
                            BackgroundColor3 = item == def and C.accent or C.border,
                            BorderSizePixel = 0,
                            ZIndex = 27,
                        }, optBtn)
                        corner(2, dot)

                        local optLbl = new("TextLabel", {
                            Size = UDim2.new(1, -22, 1, 0),
                            Position = UDim2.new(0, 20, 0, 0),
                            BackgroundTransparency = 1,
                            Text = item,
                            TextColor3 = item == def and C.accentLight or C.textDim,
                            Font = Enum.Font.Gotham,
                            TextSize = 11,
                            TextXAlignment = Enum.TextXAlignment.Left,
                            ZIndex = 27,
                        }, optBtn)

                        new("Frame", {
                            Size = UDim2.new(1, -16, 0, 1),
                            Position = UDim2.new(0, 8, 1, -1),
                            BackgroundColor3 = C.border2,
                            BorderSizePixel = 0,
                            ZIndex = 27,
                        }, optBtn)

                        optBtn.MouseEnter:Connect(function()
                            tw(optBtn, {BackgroundColor3 = Color3.fromRGB(15, 12, 35)}, 0.1)
                        end)
                        optBtn.MouseLeave:Connect(function()
                            tw(optBtn, {BackgroundColor3 = C.bg3}, 0.1)
                        end)

                        optBtn.MouseButton1Click:Connect(function()
                            lib.flags[flag] = item
                            selLbl.Text = item
                            for _, ch in ipairs(optFrame:GetChildren()) do
                                if ch:IsA("TextButton") then
                                    local d2 = ch:FindFirstChildOfClass("Frame")
                                    local l2 = ch:FindFirstChildOfClass("TextLabel")
                                    if d2 then tw(d2, {BackgroundColor3 = C.border}, 0.1) end
                                    if l2 then tw(l2, {TextColor3 = C.textDim}, 0.1) end
                                end
                            end
                            tw(dot, {BackgroundColor3 = C.accent}, 0.1)
                            tw(optLbl, {TextColor3 = C.accentLight}, 0.1)
                            if cb then pcall(cb, item) end
                            isOpen = false
                            tw(optFrame, {Size = UDim2.new(1, 0, 0, 0)}, 0.2, Enum.EasingStyle.Quart)
                            tw(arrow, {Rotation = 0}, 0.2)
                        end)
                    end

                    local ddBtn = new("TextButton", {
                        Size = UDim2.new(1, 0, 1, 0),
                        BackgroundTransparency = 1,
                        Text = "",
                        ZIndex = 23,
                    }, selFrame)
                    ddBtn.MouseButton1Click:Connect(function()
                        isOpen = not isOpen
                        if isOpen then
                            tw(optFrame, {Size = UDim2.new(1, 0, 0, totalH)}, 0.25, Enum.EasingStyle.Back)
                            tw(arrow, {Rotation = 180}, 0.2)
                        else
                            tw(optFrame, {Size = UDim2.new(1, 0, 0, 0)}, 0.18)
                            tw(arrow, {Rotation = 0}, 0.2)
                        end
                    end)

                    lib.callbacks[flag] = function(v)
                        selLbl.Text = tostring(v)
                        lib.flags[flag] = v
                    end
                end

                -- ── COLORPICKER ──
                function secObj:colorpicker(opts)
                    opts = opts or {}
                    local flag  = opts.flag or opts.name
                    local color = opts.color or Color3.fromRGB(255,255,255)
                    local cb    = opts.callback

                    lib.flags[flag] = color

                    local row = mkRow(30)
                    mkLbl(row, opts.name or "Color")

                    local right = new("Frame", {
                        Size = UDim2.new(0.44, 0, 0, 22),
                        Position = UDim2.new(0.56, 0, 0.5, -11),
                        BackgroundTransparency = 1,
                        ZIndex = 4,
                    }, row)

                    local swatch = new("Frame", {
                        Size = UDim2.new(0, 22, 0, 22),
                        Position = UDim2.new(0, 0, 0, 0),
                        BackgroundColor3 = color,
                        BorderSizePixel = 0,
                        ZIndex = 5,
                    }, right)
                    corner(5, swatch)
                    stroke(C.border, 1, swatch)

                    local hexLbl = new("TextLabel", {
                        Size = UDim2.new(1, -28, 1, 0),
                        Position = UDim2.new(0, 28, 0, 0),
                        BackgroundTransparency = 1,
                        Text = string.format("#%02X%02X%02X", color.R*255, color.G*255, color.B*255),
                        TextColor3 = C.textFaint,
                        Font = Enum.Font.GothamBold,
                        TextSize = 9,
                        ZIndex = 5,
                    }, right)

                    -- picker popup
                    local pickerOpen = false
                    local pickerFrame = nil

                    local swBtn = new("TextButton", {
                        Size = UDim2.new(1,0,1,0),
                        BackgroundTransparency = 1,
                        Text = "",
                        ZIndex = 6,
                    }, swatch)

                    swBtn.MouseButton1Click:Connect(function()
                        pickerOpen = not pickerOpen
                        if pickerFrame then pcall(function() pickerFrame:Destroy() end) pickerFrame = nil end
                        if not pickerOpen then return end

                        pickerFrame = new("Frame", {
                            Size = UDim2.new(0, 190, 0, 0),
                            Position = UDim2.new(0, 0, 1, 6),
                            BackgroundColor3 = C.bg3,
                            BorderSizePixel = 0,
                            AutomaticSize = Enum.AutomaticSize.Y,
                            ZIndex = 50,
                            ClipsDescendants = false,
                        }, swatch)
                        corner(10, pickerFrame)
                        stroke(C.border, 1, pickerFrame)
                        pad(10, 10, 10, 10, pickerFrame)
                        listLayout(8, nil, pickerFrame)

                        local r, g, b = color.R, color.G, color.B
                        local vals = {r, g, b}
                        local chData = {
                            {"R", Color3.fromRGB(255, 80, 100)},
                            {"G", Color3.fromRGB(80, 220, 120)},
                            {"B", Color3.fromRGB(80, 140, 255)},
                        }

                        local function updateColor()
                            local nc = Color3.new(
                                math.clamp(vals[1], 0, 1),
                                math.clamp(vals[2], 0, 1),
                                math.clamp(vals[3], 0, 1)
                            )
                            lib.flags[flag] = nc
                            swatch.BackgroundColor3 = nc
                            hexLbl.Text = string.format("#%02X%02X%02X",
                                math.floor(nc.R*255), math.floor(nc.G*255), math.floor(nc.B*255))
                            if cb then pcall(cb, nc) end
                        end

                        for i, ch in ipairs(chData) do
                            local chRow = new("Frame", {
                                Size = UDim2.new(1, 0, 0, 20),
                                BackgroundTransparency = 1,
                                ZIndex = 51,
                            }, pickerFrame)

                            new("TextLabel", {
                                Size = UDim2.new(0, 10, 1, 0),
                                BackgroundTransparency = 1,
                                Text = ch[1],
                                TextColor3 = ch[2],
                                Font = Enum.Font.GothamBold,
                                TextSize = 10,
                                ZIndex = 52,
                            }, chRow)

                            local ct = new("Frame", {
                                Size = UDim2.new(1, -38, 0, 3),
                                Position = UDim2.new(0, 14, 0.5, -1),
                                BackgroundColor3 = Color3.fromRGB(20,15,45),
                                BorderSizePixel = 0,
                                ZIndex = 52,
                            }, chRow)
                            corner(2, ct)

                            local cf = new("Frame", {
                                Size = UDim2.new(vals[i], 0, 1, 0),
                                BackgroundColor3 = ch[2],
                                BorderSizePixel = 0,
                                ZIndex = 53,
                            }, ct)
                            corner(2, cf)

                            local ck = new("Frame", {
                                Size = UDim2.new(0, 10, 0, 10),
                                Position = UDim2.new(vals[i], -5, 0.5, -5),
                                BackgroundColor3 = C.white,
                                BorderSizePixel = 0,
                                ZIndex = 54,
                            }, ct)
                            corner(5, ck)
                            stroke(ch[2], 1.5, ck)

                            local vl = new("TextLabel", {
                                Size = UDim2.new(0, 22, 1, 0),
                                Position = UDim2.new(1, -22, 0, 0),
                                BackgroundTransparency = 1,
                                Text = tostring(math.floor(vals[i]*255)),
                                TextColor3 = ch[2],
                                Font = Enum.Font.GothamBold,
                                TextSize = 9,
                                TextXAlignment = Enum.TextXAlignment.Right,
                                ZIndex = 52,
                            }, chRow)

                            local s2 = false
                            ct.InputBegan:Connect(function(inp)
                                if inp.UserInputType == Enum.UserInputType.MouseButton1 then s2 = true end
                            end)
                            UIS.InputEnded:Connect(function(inp)
                                if inp.UserInputType == Enum.UserInputType.MouseButton1 then s2 = false end
                            end)
                            UIS.InputChanged:Connect(function(inp)
                                if s2 and inp.UserInputType == Enum.UserInputType.MouseMovement then
                                    local p = math.clamp((inp.Position.X - ct.AbsolutePosition.X) / ct.AbsoluteSize.X, 0, 1)
                                    vals[i] = p
                                    cf.Size = UDim2.new(p, 0, 1, 0)
                                    ck.Position = UDim2.new(p, -5, 0.5, -5)
                                    vl.Text = tostring(math.floor(p*255))
                                    updateColor()
                                end
                            end)
                        end
                    end)
                end

                -- ── BUTTON ──
                function secObj:button(opts)
                    opts = opts or {}
                    local cb = opts.callback
                    local isPrimary = opts.style ~= "secondary"

                    local btn = new("TextButton", {
                        Size = UDim2.new(1, 0, 0, 28),
                        BackgroundColor3 = isPrimary and C.accentDark or C.bg3,
                        BorderSizePixel = 0,
                        Text = opts.name or "Button",
                        TextColor3 = isPrimary and Color3.fromRGB(210, 185, 255) or C.textDim,
                        Font = Enum.Font.GothamBold,
                        TextSize = 10,
                        ZIndex = 4,
                    }, body)
                    corner(8, btn)
                    stroke(isPrimary and C.border or C.border2, 1, btn)
                    if isPrimary then
                        grad(C.accentDark, Color3.fromRGB(90, 35, 200), 135, btn)
                    end

                    btn.MouseEnter:Connect(function()
                        tw(btn, {BackgroundColor3 = isPrimary and C.accent or Color3.fromRGB(15, 12, 35)}, 0.12)
                    end)
                    btn.MouseLeave:Connect(function()
                        tw(btn, {BackgroundColor3 = isPrimary and C.accentDark or C.bg3}, 0.12)
                    end)
                    btn.MouseButton1Down:Connect(function()
                        tw(btn, {Size = UDim2.new(1, 0, 0, 24)}, 0.07)
                    end)
                    btn.MouseButton1Up:Connect(function()
                        tw(btn, {Size = UDim2.new(1, 0, 0, 28)}, 0.15, Enum.EasingStyle.Back)
                        if cb then task.defer(pcall, cb) end
                    end)
                end

                -- ── KEYBIND ──
                function secObj:keybind(opts)
                    opts = opts or {}
                    local flag    = opts.flag or opts.name
                    local default = opts.default or Enum.KeyCode.Unknown
                    local cb      = opts.callback
                    local listening = false

                    lib.flags[flag] = default

                    local row = mkRow(30)
                    mkLbl(row, opts.name or "Keybind")

                    local kbFrame = new("Frame", {
                        Size = UDim2.new(0.43, 0, 0, 22),
                        Position = UDim2.new(0.57, 0, 0.5, -11),
                        BackgroundColor3 = C.bg3,
                        BorderSizePixel = 0,
                        ZIndex = 4,
                    }, row)
                    corner(5, kbFrame)
                    stroke(C.border, 1, kbFrame)

                    -- inset bottom shadow
                    new("Frame", {
                        Size = UDim2.new(1, 0, 0, 2),
                        Position = UDim2.new(0, 0, 1, -2),
                        BackgroundColor3 = Color3.fromRGB(10, 5, 30),
                        BorderSizePixel = 0,
                        ZIndex = 5,
                    }, kbFrame)

                    local kbLbl = new("TextLabel", {
                        Size = UDim2.new(1, 0, 1, 0),
                        BackgroundTransparency = 1,
                        Text = default.Name:upper(),
                        TextColor3 = C.secTitle,
                        Font = Enum.Font.GothamBold,
                        TextSize = 9,
                        ZIndex = 5,
                    }, kbFrame)

                    local kbBtn = new("TextButton", {
                        Size = UDim2.new(1, 0, 1, 0),
                        BackgroundTransparency = 1,
                        Text = "",
                        ZIndex = 6,
                    }, kbFrame)

                    kbBtn.MouseButton1Click:Connect(function()
                        listening = true
                        kbLbl.Text = "..."
                        kbLbl.TextColor3 = C.accentLight
                        tw(kbFrame, {BackgroundColor3 = Color3.fromRGB(20, 12, 50)}, 0.1)
                    end)

                    UIS.InputBegan:Connect(function(inp, gp)
                        if gp then return end
                        if listening and inp.UserInputType == Enum.UserInputType.Keyboard then
                            listening = false
                            lib.flags[flag] = inp.KeyCode
                            kbLbl.Text = inp.KeyCode.Name:upper()
                            kbLbl.TextColor3 = C.secTitle
                            tw(kbFrame, {BackgroundColor3 = C.bg3}, 0.1)
                        elseif not listening and inp.KeyCode == lib.flags[flag] then
                            if cb then pcall(cb) end
                        end
                    end)
                end

                -- ── LABEL ──
                function secObj:label(opts)
                    opts = opts or {}
                    local text = opts.text or opts.name or ""

                    local row = new("Frame", {
                        Size = UDim2.new(1, 0, 0, 22),
                        BackgroundTransparency = 1,
                        ZIndex = 3,
                    }, body)

                    local lL = new("Frame", {
                        Size = UDim2.new(0.5, -45, 0, 1),
                        Position = UDim2.new(0, 0, 0.5, 0),
                        BackgroundColor3 = C.border2,
                        BorderSizePixel = 0,
                        ZIndex = 4,
                    }, row)
                    grad(Color3.fromRGB(35, 20, 70), C.bg2, 0, lL)

                    new("TextLabel", {
                        Size = UDim2.new(0, 90, 1, 0),
                        Position = UDim2.new(0.5, -45, 0, 0),
                        BackgroundTransparency = 1,
                        Text = text:upper(),
                        TextColor3 = C.textFaint,
                        Font = Enum.Font.GothamBold,
                        TextSize = 8,
                        ZIndex = 4,
                    }, row)

                    local lR = new("Frame", {
                        Size = UDim2.new(0.5, -45, 0, 1),
                        Position = UDim2.new(0.5, 45, 0.5, 0),
                        BackgroundColor3 = C.border2,
                        BorderSizePixel = 0,
                        ZIndex = 4,
                    }, row)
                    grad(C.bg2, Color3.fromRGB(35, 20, 70), 0, lR)
                end

                -- ── TEXTBOX ──
                function secObj:textbox(opts)
                    opts = opts or {}
                    local flag = opts.flag or opts.name
                    local cb   = opts.callback

                    lib.flags[flag] = opts.default or ""

                    local row = mkRow(30)
                    mkLbl(row, opts.name or "Input")

                    local tbWrap = new("Frame", {
                        Size = UDim2.new(0.44, 0, 0, 24),
                        Position = UDim2.new(0.56, 0, 0.5, -12),
                        BackgroundColor3 = C.bg3,
                        BorderSizePixel = 0,
                        ZIndex = 4,
                    }, row)
                    corner(7, tbWrap)
                    local tbStroke = stroke(C.border, 1, tbWrap)

                    local tb2 = new("TextBox", {
                        Size = UDim2.new(1, -12, 1, 0),
                        Position = UDim2.new(0, 8, 0, 0),
                        BackgroundTransparency = 1,
                        Text = opts.default or "",
                        PlaceholderText = opts.placeholder or "type here...",
                        TextColor3 = C.text,
                        PlaceholderColor3 = C.textFaint,
                        Font = Enum.Font.Gotham,
                        TextSize = 11,
                        TextXAlignment = Enum.TextXAlignment.Left,
                        ClearTextOnFocus = opts.clearOnFocus ~= false,
                        ZIndex = 5,
                    }, tbWrap)

                    tb2.Focused:Connect(function()
                        tw(tbWrap, {BackgroundColor3 = Color3.fromRGB(14, 10, 35)}, 0.15)
                        if tbStroke then tbStroke.Color = C.accent end
                    end)
                    tb2.FocusLost:Connect(function(enter)
                        tw(tbWrap, {BackgroundColor3 = C.bg3}, 0.15)
                        if tbStroke then tbStroke.Color = C.border end
                        lib.flags[flag] = tb2.Text
                        if cb then pcall(cb, tb2.Text, enter) end
                    end)
                end

                -- ── DIVIDER ──
                function secObj:divider()
                    local d = new("Frame", {
                        Size = UDim2.new(1, 0, 0, 1),
                        BackgroundColor3 = C.border2,
                        BorderSizePixel = 0,
                        ZIndex = 3,
                    }, body)
                    grad(Color3.fromRGB(8, 6, 20), Color3.fromRGB(45, 25, 90), 0, d)
                end

                return secObj
            end

            -- ── CONFIG SECTION ──
            function colObj:configSection()
                local sec = self:section({name = "Config"})
                sec:label({text = "SAVED CONFIGS"})

                -- config list holder
                local listHolder = new("Frame", {
                    Size = UDim2.new(1, 0, 0, 0),
                    BackgroundTransparency = 1,
                    AutomaticSize = Enum.AutomaticSize.Y,
                    ZIndex = 4,
                }, inner) -- attach to scroll inner
                listLayout(4, nil, listHolder)

                local function refreshList()
                    for _, c in ipairs(listHolder:GetChildren()) do
                        if c:IsA("Frame") then c:Destroy() end
                    end
                    for _, cfgName in ipairs(lib:listConfigs()) do
                        local item = new("Frame", {
                            Size = UDim2.new(1, 0, 0, 28),
                            BackgroundColor3 = Color3.fromRGB(14, 11, 34),
                            BorderSizePixel = 0,
                            ZIndex = 5,
                        }, listHolder)
                        corner(7, item)
                        stroke(C.border2, 1, item)

                        local dot = new("Frame", {
                            Size = UDim2.new(0, 5, 0, 5),
                            Position = UDim2.new(0, 9, 0.5, -2),
                            BackgroundColor3 = C.accent,
                            BorderSizePixel = 0,
                            ZIndex = 6,
                        }, item)
                        corner(3, dot)

                        new("TextLabel", {
                            Size = UDim2.new(1, -60, 1, 0),
                            Position = UDim2.new(0, 20, 0, 0),
                            BackgroundTransparency = 1,
                            Text = cfgName .. ".json",
                            TextColor3 = C.text,
                            Font = Enum.Font.Gotham,
                            TextSize = 11,
                            TextXAlignment = Enum.TextXAlignment.Left,
                            ZIndex = 6,
                        }, item)

                        local loadB = new("TextButton", {
                            Size = UDim2.new(0, 22, 0, 22),
                            Position = UDim2.new(1, -46, 0.5, -11),
                            BackgroundColor3 = Color3.fromRGB(20, 15, 50),
                            BorderSizePixel = 0,
                            Text = "↑",
                            TextColor3 = C.accentLight,
                            Font = Enum.Font.GothamBold,
                            TextSize = 12,
                            ZIndex = 7,
                        }, item)
                        corner(5, loadB)
                        loadB.MouseButton1Click:Connect(function()
                            lib:loadConfig(cfgName)
                        end)

                        local delB = new("TextButton", {
                            Size = UDim2.new(0, 22, 0, 22),
                            Position = UDim2.new(1, -20, 0.5, -11),
                            BackgroundColor3 = Color3.fromRGB(20, 15, 50),
                            BorderSizePixel = 0,
                            Text = "✕",
                            TextColor3 = C.textDim,
                            Font = Enum.Font.GothamBold,
                            TextSize = 10,
                            ZIndex = 7,
                        }, item)
                        corner(5, delB)
                        delB.MouseButton1Click:Connect(function()
                            lib:deleteConfig(cfgName)
                            refreshList()
                        end)
                    end
                end

                refreshList()

                -- name textbox
                local cfgNameFlag = "___cfg_name"
                lib.flags[cfgNameFlag] = "default"

                sec:textbox({
                    name = "Name",
                    flag = cfgNameFlag,
                    placeholder = "config name...",
                    clearOnFocus = false,
                    callback = function(v) lib.flags[cfgNameFlag] = v end
                })

                -- buttons row
                local btnRow = new("Frame", {
                    Size = UDim2.new(1, 0, 0, 26),
                    BackgroundTransparency = 1,
                    ZIndex = 4,
                }, inner)
                listLayout(5, Enum.FillDirection.Horizontal, btnRow)

                local function cfgBtn(text, col, cb)
                    local b = new("TextButton", {
                        Size = UDim2.new(0.33, -4, 1, 0),
                        BackgroundColor3 = Color3.fromRGB(12, 10, 28),
                        BorderSizePixel = 0,
                        Text = text,
                        TextColor3 = col or C.textDim,
                        Font = Enum.Font.GothamBold,
                        TextSize = 9,
                        ZIndex = 5,
                    }, btnRow)
                    corner(6, b)
                    stroke(C.border2, 1, b)
                    b.MouseButton1Click:Connect(function()
                        tw(b, {BackgroundColor3 = C.border2}, 0.1)
                        task.delay(0.1, function() tw(b, {BackgroundColor3 = Color3.fromRGB(12, 10, 28)}, 0.1) end)
                        if cb then pcall(cb) end
                    end)
                    return b
                end

                cfgBtn("SAVE", C.accentLight, function()
                    lib:saveConfig(lib.flags[cfgNameFlag] or "default")
                    refreshList()
                end)
                cfgBtn("LOAD", C.text, function()
                    lib:loadConfig(lib.flags[cfgNameFlag] or "default")
                end)
                cfgBtn("NEW", C.textDim, function()
                    lib:saveConfig(lib.flags[cfgNameFlag] or "new_config")
                    refreshList()
                end)
            end

            return colObj
        end

        return tabObj
    end

    function winObj:notify(t, m, i, d)
        lib:notify(t, m, i, d)
    end

    function winObj:setWatermark(t)
        lib:setWatermark(t)
    end

    -- welcome notif
    task.delay(0.6, function()
        lib:notify("LOADED", name .. " ready!", "✦", 4)
    end)

    return winObj
end

return lib
