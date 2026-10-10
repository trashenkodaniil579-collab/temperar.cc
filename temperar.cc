--[[
██████╗ █████╗ ██╗ ██████╗ ██╗
██╔══██╗██╔══██╗██║ ██╔═══██╗██║
██║ ██║███████║██║ ██║ ██║██║
██║ ██║██╔══██║██║ ██║ ██║██║
██████╔╝██║ ██║███████╗╚██████╔╝███████╗
╚═════╝ ╚═╝ ╚═╝╚══════╝ ╚═════╝ ╚══════╝
dalol.xyz UI Library v1.0
by dalol
--]]

local dalol = {}
dalol.__index = dalol

-- ─────────────────────────────────────────────
-- SERVICES
-- ─────────────────────────────────────────────
local TweenService = game:GetService("TweenService")
local UserInputService = game:GetService("UserInputService")
local RunService = game:GetService("RunService")
local Players = game:GetService("Players")
local lp = Players.LocalPlayer
local mouse = lp:GetMouse()

-- ─────────────────────────────────────────────
-- THEME
-- ─────────────────────────────────────────────
local T = {
bg = Color3.fromHex("0b091e"),
bg2 = Color3.fromHex("0e0c22"),
bg3 = Color3.fromHex("0b0920"),
border = Color3.fromHex("1c1830"),
border2 = Color3.fromHex("251a4a"),
accent = Color3.fromHex("7b2fff"),
accent2 = Color3.fromHex("5b1fd1"),
accent3 = Color3.fromHex("8844ff"),
text = Color3.fromHex("8888bb"),
text2 = Color3.fromHex("c8b8ff"),
text3 = Color3.fromHex("3a3460"),
secTitle = Color3.fromHex("7040cc"),
white = Color3.fromHex("ffffff"),
red = Color3.fromHex("ff5f57"),
yellow = Color3.fromHex("febc2e"),
green = Color3.fromHex("28c840"),
notifBg = Color3.fromHex("0e0c20"),
}

-- ─────────────────────────────────────────────
-- HELPERS
-- ─────────────────────────────────────────────
local function tween(obj, props, t, style, dir)
style = style or Enum.EasingStyle.Quart
dir = dir or Enum.EasingDirection.Out
local info = TweenInfo.new(t or 0.25, style, dir)
TweenService:Create(obj, info, props):Play()
end

local function makeInst(cls, props, parent)
local obj = Instance.new(cls)
for k,v in pairs(props) do obj[k] = v end
if parent then obj.Parent = parent end
return obj
end

local function corner(r, parent)
return makeInst("UICorner", {CornerRadius = UDim.new(0, r)}, parent)
end

local function stroke(c, t, parent)
return makeInst("UIStroke", {Color = c, Thickness = t or 1, ApplyStrokeMode = Enum.ApplyStrokeMode.Border}, parent)
end

local function gradient(c0, c1, rot, parent)
local g = makeInst("UIGradient", {Rotation = rot or 90}, parent)
g.Color = ColorSequence.new(c0, c1)
return g
end

local function padding(t, b, l, r, parent)
return makeInst("UIPadding", {
PaddingTop = UDim.new(0, t or 0),
PaddingBottom = UDim.new(0, b or 0),
PaddingLeft = UDim.new(0, l or 0),
PaddingRight = UDim.new(0, r or 0),
}, parent)
end

local function list(spacing, parent)
return makeInst("UIListLayout", {
SortOrder = Enum.SortOrder.LayoutOrder,
Padding = UDim.new(0, spacing or 0),
}, parent)
end

local function autoSize(frame, axis)
frame.AutomaticSize = axis or Enum.AutomaticSize.Y
end

-- ─────────────────────────────────────────────
-- FLAGS (config state)
-- ─────────────────────────────────────────────
dalol.flags = {}
dalol.callbacks = {}

-- ─────────────────────────────────────────────
-- CONFIG SYSTEM
-- ─────────────────────────────────────────────
local CONFIG_PATH = "dalol_configs/"

local function ensureFolder()
if not isfolder(CONFIG_PATH) then makefolder(CONFIG_PATH) end
end

function dalol:saveConfig(name)
ensureFolder()
local data = {}
for k, v in pairs(self.flags) do
if type(v) == "number" or type(v) == "boolean" or type(v) == "string" then
data[k] = v
elseif typeof(v) == "Color3" then
data[k] = {r = v.R, g = v.G, b = v.B}
end
end
writefile(CONFIG_PATH .. name .. ".json", game:GetService("HttpService"):JSONEncode(data))
self:notify("CONFIG SAVED", name .. ".json saved", "✓")
end

function dalol:loadConfig(name)
ensureFolder()
local path = CONFIG_PATH .. name .. ".json"
if not isfile(path) then self:notify("ERROR", "Config not found", "✕") return end
local ok, data = pcall(function()
return game:GetService("HttpService"):JSONDecode(readfile(path))
end)
if not ok then self:notify("ERROR", "Failed to parse config", "✕") return end
for k, v in pairs(data) do
if type(v) == "table" and v.r then
self.flags[k] = Color3.new(v.r, v.g, v.b)
else
self.flags[k] = v
end
if self.callbacks[k] then
pcall(self.callbacks[k], self.flags[k])
end
end
self:notify("CONFIG LOADED", name .. ".json loaded", "✓")
end

function dalol:listConfigs()
ensureFolder()
local files = {}
for _, f in ipairs(listfiles(CONFIG_PATH)) do
local name = f:gsub(CONFIG_PATH, ""):gsub("%.json$", "")
table.insert(files, name)
end
return files
end

-- ─────────────────────────────────────────────
-- NOTIFICATIONS
-- ─────────────────────────────────────────────
local notifHolder

local function ensureNotifHolder(gui)
if notifHolder then return end
notifHolder = makeInst("Frame", {
Name = "Notifications",
Size = UDim2.new(0, 230, 1, 0),
Position = UDim2.new(1, -244, 0, 0),
BackgroundTransparency = 1,
ZIndex = 999,
}, gui)
local l = list(8, notifHolder)
l.HorizontalAlignment = Enum.HorizontalAlignment.Center
l.VerticalAlignment = Enum.VerticalAlignment.Bottom
padding(0, 14, 0, 0, notifHolder)
end

function dalol:notify(title, message, icon, duration)
duration = duration or 3.5
local gui = self._gui
ensureNotifHolder(gui)

local card = makeInst("Frame", {
    Size = UDim2.new(1, 0, 0, 0),
    AutomaticSize = Enum.AutomaticSize.Y,
    BackgroundColor3 = T.notifBg,
    BorderSizePixel = 0,
    ZIndex = 999,
    ClipsDescendants = true,
}, notifHolder)
corner(9, card)

-- left accent bar
makeInst("Frame", {
    Size = UDim2.new(0, 3, 1, 0),
    BackgroundColor3 = T.accent,
    BorderSizePixel = 0,
    ZIndex = 1000,
}, card)

local inner = makeInst("Frame", {
    Size = UDim2.new(1, -3, 1, 0),
    Position = UDim2.new(0, 3, 0, 0),
    BackgroundTransparency = 1,
    AutomaticSize = Enum.AutomaticSize.Y,
    ZIndex = 1000,
}, card)
padding(10, 10, 10, 10, inner)
list(4, inner)

-- icon + title row
local topRow = makeInst("Frame", {
    Size = UDim2.new(1, 0, 0, 16),
    BackgroundTransparency = 1,
    ZIndex = 1000,
}, inner)
if icon then
    makeInst("TextLabel", {
        Size = UDim2.new(0, 16, 1, 0),
        BackgroundTransparency = 1,
        Text = icon,
        TextColor3 = T.accent3,
        Font = Enum.Font.GothamBold,
        TextSize = 13,
        ZIndex = 1000,
    }, topRow)
end
makeInst("TextLabel", {
    Size = UDim2.new(1, -20, 1, 0),
    Position = UDim2.new(0, 20, 0, 0),
    BackgroundTransparency = 1,
    Text = title,
    TextColor3 = T.text2,
    Font = Enum.Font.Code,
    TextSize = 11,
    TextXAlignment = Enum.TextXAlignment.Left,
    ZIndex = 1000,
}, topRow)

makeInst("TextLabel", {
    Size = UDim2.new(1, 0, 0, 14),
    BackgroundTransparency = 1,
    Text = message,
    TextColor3 = T.text3,
    Font = Enum.Font.Gotham,
    TextSize = 12,
    TextXAlignment = Enum.TextXAlignment.Left,
    TextWrapped = true,
    ZIndex = 1000,
}, inner)

-- progress bar
local barBg = makeInst("Frame", {
    Size = UDim2.new(1, 0, 0, 2),
    BackgroundColor3 = T.border,
    BorderSizePixel = 0,
    ZIndex = 1000,
}, inner)
corner(2, barBg)
local barFill = makeInst("Frame", {
    Size = UDim2.new(1, 0, 1, 0),
    BackgroundColor3 = T.accent,
    BorderSizePixel = 0,
    ZIndex = 1001,
}, barBg)
corner(2, barFill)
gradient(T.accent2, T.accent3, 0, barFill)

stroke(T.border2, 1, card)

-- animate in
card.BackgroundTransparency = 1
tween(card, {BackgroundTransparency = 0}, 0.3)

-- progress drain
tween(barFill, {Size = UDim2.new(0, 0, 1, 0)}, duration, Enum.EasingStyle.Linear, Enum.EasingDirection.In)

task.delay(duration, function()
    tween(card, {BackgroundTransparency = 1}, 0.3)
    task.wait(0.35)
    card:Destroy()
end)
end

-- ─────────────────────────────────────────────
-- WATERMARK
-- ─────────────────────────────────────────────
function dalol:setWatermark(text)
if self._wmLabel then
self._wmLabel.Text = text
end
end

local function buildWatermark(gui, text)
local frame = makeInst("Frame", {
Name = "Watermark",
Size = UDim2.new(0, 0, 0, 28),
Position = UDim2.new(0, 14, 0, 14),
AutomaticSize = Enum.AutomaticSize.X,
BackgroundColor3 = T.bg3,
BorderSizePixel = 0,
ZIndex = 100,
}, gui)
corner(7, frame)
stroke(T.border2, 1, frame)

local accentBar = makeInst("Frame", {
    Size = UDim2.new(0, 3, 1, 0),
    BackgroundColor3 = T.accent,
    BorderSizePixel = 0,
    ZIndex = 101,
}, frame)
corner(2, accentBar)

local dot = makeInst("Frame", {
    Size = UDim2.new(0, 7, 0, 7),
    Position = UDim2.new(0, 10, 0.5, -3),
    BackgroundColor3 = T.accent3,
    BorderSizePixel = 0,
    ZIndex = 101,
}, frame)
corner(4, dot)

local lbl = makeInst("TextLabel", {
    Size = UDim2.new(0, 0, 1, 0),
    Position = UDim2.new(0, 23, 0, 0),
    AutomaticSize = Enum.AutomaticSize.X,
    BackgroundTransparency = 1,
    Text = text or "dalol.xyz",
    TextColor3 = T.accent3,
    Font = Enum.Font.Code,
    TextSize = 12,
    ZIndex = 101,
}, frame)
padding(0, 0, 23, 12, frame)

-- glow effect
makeInst("ImageLabel", {
    Size = UDim2.new(1, 20, 1, 20),
    Position = UDim2.new(0, -10, 0, -10),
    BackgroundTransparency = 1,
    Image = "rbxassetid://5028857472",
    ImageColor3 = T.accent,
    ImageTransparency = 0.85,
    ZIndex = 99,
    ScaleType = Enum.ScaleType.Slice,
    SliceCenter = Rect.new(24, 24, 276, 276),
}, frame)

return lbl
end

-- ─────────────────────────────────────────────
-- DRAGGING
-- ─────────────────────────────────────────────
local function makeDraggable(handle, target)
local dragging, dragInput, startPos, startObjPos

handle.InputBegan:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 then
        dragging = true
        startPos = input.Position
        startObjPos = target.Position
    end
end)

handle.InputEnded:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 then
        dragging = false
    end
end)

UserInputService.InputChanged:Connect(function(input)
    if dragging and input.UserInputType == Enum.UserInputType.MouseMovement then
        local delta = input.Position - startPos
        target.Position = UDim2.new(
            startObjPos.X.Scale,
            startObjPos.X.Offset + delta.X,
            startObjPos.Y.Scale,
            startObjPos.Y.Offset + delta.Y
        )
    end
end)
end

-- ─────────────────────────────────────────────
-- WINDOW
-- ─────────────────────────────────────────────
function dalol:window(opts)
opts = opts or {}
local name = opts.name or "dalol.xyz"
local width = opts.width or 760
local height = opts.height or 520
local watermark = opts.watermark ~= false

-- ScreenGui
local gui = makeInst("ScreenGui", {
    Name             = "DalolUI",
    ResetOnSpawn     = false,
    ZIndexBehavior   = Enum.ZIndexBehavior.Sibling,
})
pcall(function() gui.Parent = game:GetService("CoreGui") end)
if not gui.Parent then gui.Parent = lp:WaitForChild("PlayerGui") end
self._gui = gui

-- Blur background
local blur = makeInst("BlurEffect", {Size = 0}, game:GetService("Lighting"))
tween(blur, {Size = 12}, 0.4)

-- Main window frame
local win = makeInst("Frame", {
    Name             = "Window",
    Size             = UDim2.new(0, width, 0, height),
    Position         = UDim2.new(0.5, -width/2, 0.5, -height/2),
    BackgroundColor3 = T.bg,
    BorderSizePixel  = 0,
    ClipsDescendants = true,
}, gui)
corner(14, win)
stroke(T.border2, 1, win)
self._win = win

-- glow shadow
makeInst("ImageLabel", {
    Size = UDim2.new(1, 40, 1, 40),
    Position = UDim2.new(0, -20, 0, -20),
    BackgroundTransparency = 1,
    Image = "rbxassetid://5028857472",
    ImageColor3 = T.accent,
    ImageTransparency = 0.7,
    ZIndex = -1,
    ScaleType = Enum.ScaleType.Slice,
    SliceCenter = Rect.new(24, 24, 276, 276),
}, win)

-- OPEN ANIMATION: scale + fade
win.Size = UDim2.new(0, width, 0, 0)
win.BackgroundTransparency = 1
tween(win, {
    Size = UDim2.new(0, width, 0, height),
    BackgroundTransparency = 0,
}, 0.45, Enum.EasingStyle.Back, Enum.EasingDirection.Out)

-- ── TITLEBAR ──
local tb = makeInst("Frame", {
    Name             = "Titlebar",
    Size             = UDim2.new(1, 0, 0, 48),
    BackgroundColor3 = T.bg3,
    BorderSizePixel  = 0,
    ZIndex           = 10,
}, win)
gradient(Color3.fromHex("110e28"), Color3.fromHex("1a1438"), 0, tb)

makeInst("Frame", {
    Size = UDim2.new(1, 0, 0, 1),
    Position = UDim2.new(0, 0, 1, -1),
    BackgroundColor3 = T.border,
    BorderSizePixel = 0,
    ZIndex = 11,
}, tb)

-- Logo
local logo = makeInst("Frame", {
    Size = UDim2.new(0, 28, 0, 28),
    Position = UDim2.new(0, 14, 0.5, -14),
    BackgroundColor3 = T.accent,
    BorderSizePixel = 0,
    ZIndex = 11,
}, tb)
corner(8, logo)
gradient(T.accent2, T.accent3, 135, logo)
makeInst("TextLabel", {
    Size = UDim2.new(1, 0, 1, 0),
    BackgroundTransparency = 1,
    Text = "✦",
    TextColor3 = T.white,
    Font = Enum.Font.GothamBold,
    TextSize = 14,
    ZIndex = 12,
}, logo)

-- Title text
makeInst("TextLabel", {
    Size = UDim2.new(0, 200, 0, 18),
    Position = UDim2.new(0, 50, 0, 8),
    BackgroundTransparency = 1,
    Text = name:upper(),
    TextColor3 = T.text2,
    Font = Enum.Font.Code,
    TextSize = 13,
    TextXAlignment = Enum.TextXAlignment.Left,
    ZIndex = 11,
}, tb)

makeInst("TextLabel", {
    Size = UDim2.new(0, 200, 0, 14),
    Position = UDim2.new(0, 50, 0, 26),
    BackgroundTransparency = 1,
    Text = os.date("%b %d %Y") .. " · v1.0",
    TextColor3 = T.text3,
    Font = Enum.Font.Gotham,
    TextSize = 11,
    TextXAlignment = Enum.TextXAlignment.Left,
    ZIndex = 11,
}, tb)

-- Window buttons
local btnData = {
    {color = T.red,    x = width - 20},
    {color = T.yellow, x = width - 36},
    {color = T.green,  x = width - 52},
}
for _, b in ipairs(btnData) do
    local btn = makeInst("Frame", {
        Size = UDim2.new(0, 12, 0, 12),
        Position = UDim2.new(0, b.x, 0.5, -6),
        BackgroundColor3 = b.color,
        BorderSizePixel = 0,
        ZIndex = 11,
    }, tb)
    corner(6, btn)
end

-- Close on red button click
local closeBtn = makeInst("TextButton", {
    Size = UDim2.new(0, 12, 0, 12),
    Position = UDim2.new(0, width - 20, 0.5, -6),
    BackgroundTransparency = 1,
    Text = "",
    ZIndex = 12,
}, tb)
closeBtn.MouseButton1Click:Connect(function()
    tween(win, {Size = UDim2.new(0, width, 0, 0), BackgroundTransparency = 1}, 0.3, Enum.EasingStyle.Quart)
    tween(blur, {Size = 0}, 0.4)
    task.wait(0.35)
    gui:Destroy()
end)

makeDraggable(tb, win)

-- ── TABS BAR ──
local tabsBar = makeInst("Frame", {
    Name = "TabsBar",
    Size = UDim2.new(1, 0, 0, 36),
    Position = UDim2.new(0, 0, 0, 48),
    BackgroundColor3 = Color3.fromHex("090717"),
    BorderSizePixel = 0,
    ZIndex = 9,
}, win)
makeInst("Frame", {
    Size = UDim2.new(1, 0, 0, 1),
    Position = UDim2.new(0, 0, 1, -1),
    BackgroundColor3 = T.border,
    BorderSizePixel = 0,
    ZIndex = 10,
}, tabsBar)

local tabsLayout = makeInst("UIListLayout", {
    FillDirection = Enum.FillDirection.Horizontal,
    SortOrder = Enum.SortOrder.LayoutOrder,
    Padding = UDim.new(0, 0),
}, tabsBar)
padding(0, 0, 14, 0, tabsBar)

-- ── TAB CONTENT AREA ──
local contentArea = makeInst("Frame", {
    Name = "Content",
    Size = UDim2.new(1, 0, 1, -84),
    Position = UDim2.new(0, 0, 0, 84),
    BackgroundTransparency = 1,
    ClipsDescendants = true,
}, win)

-- ── WATERMARK ──
if watermark then
    self._wmLabel = buildWatermark(gui, name)
end

-- Tab management
local tabs = {}
local activeTab = nil
self._tabs = tabs

local winObj = {}

function winObj:tab(tabOpts)
    tabOpts = tabOpts or {}
    local tabName = tabOpts.name or "Tab"

    -- Tab button
    local tabBtn = makeInst("TextButton", {
        Name = tabName,
        Size = UDim2.new(0, 0, 1, 0),
        AutomaticSize = Enum.AutomaticSize.X,
        BackgroundTransparency = 1,
        Text = "",
        ZIndex = 10,
        LayoutOrder = #tabs + 1,
    }, tabsBar)
    padding(0, 2, 18, 18, tabBtn)

    local tabLbl = makeInst("TextLabel", {
        Size = UDim2.new(1, 0, 1, 0),
        BackgroundTransparency = 1,
        Text = tabName:upper(),
        TextColor3 = T.text3,
        Font = Enum.Font.Code,
        TextSize = 11,
        ZIndex = 11,
    }, tabBtn)

    -- Active indicator dot
    local tabDot = makeInst("Frame", {
        Size = UDim2.new(0, 5, 0, 5),
        Position = UDim2.new(0.5, -2, 1, -3),
        BackgroundColor3 = T.accent,
        BorderSizePixel = 0,
        BackgroundTransparency = 1,
        ZIndex = 12,
    }, tabBtn)
    corner(3, tabDot)

    -- Active underline
    local tabLine = makeInst("Frame", {
        Size = UDim2.new(0, 0, 0, 2),
        Position = UDim2.new(0.5, 0, 1, -2),
        BackgroundColor3 = T.accent,
        BorderSizePixel = 0,
        ZIndex = 11,
    }, tabBtn)

    -- Tab content frame
    local tabFrame = makeInst("Frame", {
        Name = tabName .. "Frame",
        Size = UDim2.new(1, 0, 1, 0),
        BackgroundTransparency = 1,
        Visible = false,
        ZIndex = 5,
    }, contentArea)

    local colHolder = makeInst("Frame", {
        Size = UDim2.new(1, -28, 1, -28),
        Position = UDim2.new(0, 14, 0, 14),
        BackgroundTransparency = 1,
        AutomaticSize = Enum.AutomaticSize.None,
    }, tabFrame)
    local colList = makeInst("UIListLayout", {
        FillDirection = Enum.FillDirection.Horizontal,
        SortOrder = Enum.SortOrder.LayoutOrder,
        Padding = UDim.new(0, 10),
    }, colHolder)

    local tabEntry = {
        btn    = tabBtn,
        label  = tabLbl,
        line   = tabLine,
        dot    = tabDot,
        frame  = tabFrame,
        cols   = colHolder,
    }
    table.insert(tabs, tabEntry)

    local function activate()
        -- deactivate all
        for _, t in ipairs(tabs) do
            tween(t.label, {TextColor3 = T.text3}, 0.2)
            tween(t.line,  {Size = UDim2.new(0, 0, 0, 2)}, 0.2, Enum.EasingStyle.Quart)
            tween(t.dot,   {BackgroundTransparency = 1}, 0.2)
            t.frame.Visible = false
        end
        -- activate this
        tween(tabLbl, {TextColor3 = T.accent3}, 0.2)
        tween(tabLine, {Size = UDim2.new(1, 0, 0, 2)}, 0.25, Enum.EasingStyle.Back)
        tween(tabDot,  {BackgroundTransparency = 0}, 0.2)
        -- slide animation
        tabFrame.Position = UDim2.new(0.05, 0, 0, 0)
        tabFrame.BackgroundTransparency = 1
        tabFrame.Visible = true
        tween(tabFrame, {Position = UDim2.new(0, 0, 0, 0), BackgroundTransparency = 0}, 0.22, Enum.EasingStyle.Quart)
        activeTab = tabEntry
    end

    tabBtn.MouseButton1Click:Connect(activate)

    -- auto-activate first tab
    if #tabs == 1 then
        task.defer(activate)
    end

    -- ── COLUMN BUILDER ──
    local tabObj = {}

    function tabObj:column()
        local col = makeInst("Frame", {
            Size = UDim2.new(0.5, -5, 1, 0),
            BackgroundTransparency = 1,
            LayoutOrder = 1,
            AutomaticSize = Enum.AutomaticSize.Y,
        }, colHolder)
        list(10, col)

        local colObj = {}

        -- ────────────────────────────────
        --  SECTION
        -- ────────────────────────────────
        function colObj:section(secOpts)
            secOpts = secOpts or {}
            local secName = secOpts.name or "Section"

            local sec = makeInst("Frame", {
                Size = UDim2.new(1, 0, 0, 0),
                BackgroundColor3 = T.bg2,
                BorderSizePixel = 0,
                AutomaticSize = Enum.AutomaticSize.Y,
            }, col)
            corner(10, sec)
            stroke(T.border, 1, sec)

            -- Header
            local hdr = makeInst("Frame", {
                Size = UDim2.new(1, 0, 0, 30),
                BackgroundColor3 = Color3.fromHex("0b0920"),
                BorderSizePixel = 0,
                ZIndex = 2,
            }, sec)

            makeInst("Frame", {
                Size = UDim2.new(1, 0, 0, 1),
                Position = UDim2.new(0, 0, 1, -1),
                BackgroundColor3 = T.border,
                BorderSizePixel = 0,
                ZIndex = 3,
            }, hdr)

            -- Accent bar
            makeInst("Frame", {
                Size = UDim2.new(0, 3, 0, 13),
                Position = UDim2.new(0, 10, 0.5, -6),
                BackgroundColor3 = T.accent,
                BorderSizePixel = 0,
                ZIndex = 3,
            }, hdr)
            -- glow on accent bar
            makeInst("Frame", {
                Size = UDim2.new(0, 3, 0, 13),
                Position = UDim2.new(0, 10, 0.5, -6),
                BackgroundColor3 = T.accent3,
                BorderSizePixel = 0,
                ZIndex = 3,
                BackgroundTransparency = 0.6,
            }, hdr)

            makeInst("TextLabel", {
                Size = UDim2.new(1, -24, 1, 0),
                Position = UDim2.new(0, 20, 0, 0),
                BackgroundTransparency = 1,
                Text = secName:upper(),
                TextColor3 = T.secTitle,
                Font = Enum.Font.Code,
                TextSize = 11,
                TextXAlignment = Enum.TextXAlignment.Left,
                ZIndex = 3,
            }, hdr)

            -- Body
            local body = makeInst("Frame", {
                Size = UDim2.new(1, 0, 0, 0),
                Position = UDim2.new(0, 0, 0, 30),
                BackgroundTransparency = 1,
                AutomaticSize = Enum.AutomaticSize.Y,
                ZIndex = 2,
            }, sec)
            padding(8, 8, 10, 10, body)
            list(5, body)

            local secObj = {}

            -- ── helpers for rows ──
            local function makeRow(parent)
                local row = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 28),
                    BackgroundTransparency = 1,
                    ZIndex = 3,
                }, parent or body)
                return row
            end

            local function makeLabel(parent, text, color, font, size, xAlign)
                return makeInst("TextLabel", {
                    Size = UDim2.new(0.55, 0, 1, 0),
                    BackgroundTransparency = 1,
                    Text = text,
                    TextColor3 = color or T.text,
                    Font = font or Enum.Font.Gotham,
                    TextSize = size or 13,
                    TextXAlignment = xAlign or Enum.TextXAlignment.Left,
                    ZIndex = 4,
                }, parent)
            end

            -- ── TOGGLE ──
            function secObj:toggle(opts)
                opts = opts or {}
                local flag = opts.flag or opts.name
                local cb   = opts.callback
                dalol.flags[flag] = opts.default or false

                local row = makeRow()
                makeLabel(row, opts.name or "Toggle")

                local track = makeInst("Frame", {
                    Size = UDim2.new(0, 38, 0, 20),
                    Position = UDim2.new(1, -38, 0.5, -10),
                    BackgroundColor3 = Color3.fromHex("181530"),
                    BorderSizePixel = 0,
                    ZIndex = 4,
                }, row)
                corner(10, track)
                stroke(T.border2, 1, track)

                local knob = makeInst("Frame", {
                    Size = UDim2.new(0, 14, 0, 14),
                    Position = UDim2.new(0, 2, 0.5, -7),
                    BackgroundColor3 = T.white,
                    BorderSizePixel = 0,
                    ZIndex = 5,
                }, track)
                corner(7, knob)

                local function setState(v, animate)
                    dalol.flags[flag] = v
                    if v then
                        if animate ~= false then
                            tween(track, {BackgroundColor3 = T.accent2}, 0.2)
                            tween(knob, {Position = UDim2.new(0, 22, 0.5, -7), BackgroundColor3 = T.white}, 0.2, Enum.EasingStyle.Back)
                        else
                            track.BackgroundColor3 = T.accent2
                            knob.Position = UDim2.new(0, 22, 0.5, -7)
                        end
                    else
                        if animate ~= false then
                            tween(track, {BackgroundColor3 = Color3.fromHex("181530")}, 0.2)
                            tween(knob, {Position = UDim2.new(0, 2, 0.5, -7), BackgroundColor3 = T.white}, 0.2, Enum.EasingStyle.Back)
                        else
                            track.BackgroundColor3 = Color3.fromHex("181530")
                            knob.Position = UDim2.new(0, 2, 0.5, -7)
                        end
                    end
                    if cb then pcall(cb, v) end
                end

                if opts.default then setState(true, false) end

                dalol.callbacks[flag] = function(v) setState(v, false) end

                local btn = makeInst("TextButton", {
                    Size = UDim2.new(1, 0, 1, 0),
                    BackgroundTransparency = 1,
                    Text = "",
                    ZIndex = 6,
                }, row)
                btn.MouseButton1Click:Connect(function()
                    -- spring animation
                    tween(knob, {Size = UDim2.new(0, 10, 0, 14)}, 0.08)
                    task.delay(0.08, function()
                        tween(knob, {Size = UDim2.new(0, 14, 0, 14)}, 0.12, Enum.EasingStyle.Back)
                    end)
                    setState(not dalol.flags[flag])
                end)

                local togObj = {}
                -- colorpicker sub-element
                function togObj:colorpicker(cpOpts)
                    secObj:colorpicker(cpOpts)
                    return togObj
                end
                return togObj
            end

            -- ── SLIDER ──
            function secObj:slider(opts)
                opts = opts or {}
                local flag   = opts.flag or opts.name
                local min    = opts.min or 0
                local max    = opts.max or 100
                local default = opts.default or min
                local suffix = opts.suffix or ""
                local cb     = opts.callback

                dalol.flags[flag] = default

                local row = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 36),
                    BackgroundTransparency = 1,
                    ZIndex = 3,
                }, body)

                makeInst("TextLabel", {
                    Size = UDim2.new(1, 0, 0, 16),
                    BackgroundTransparency = 1,
                    Text = opts.name or "Slider",
                    TextColor3 = T.text,
                    Font = Enum.Font.Gotham,
                    TextSize = 13,
                    TextXAlignment = Enum.TextXAlignment.Left,
                    ZIndex = 4,
                }, row)

                local valLbl = makeInst("TextLabel", {
                    Size = UDim2.new(1, 0, 0, 16),
                    BackgroundTransparency = 1,
                    Text = tostring(default) .. suffix,
                    TextColor3 = T.secTitle,
                    Font = Enum.Font.Code,
                    TextSize = 11,
                    TextXAlignment = Enum.TextXAlignment.Right,
                    ZIndex = 4,
                }, row)

                local track = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 4),
                    Position = UDim2.new(0, 0, 0, 24),
                    BackgroundColor3 = Color3.fromHex("181530"),
                    BorderSizePixel = 0,
                    ZIndex = 4,
                }, row)
                corner(3, track)

                local fill = makeInst("Frame", {
                    Size = UDim2.new((default - min) / (max - min), 0, 1, 0),
                    BackgroundColor3 = T.accent,
                    BorderSizePixel = 0,
                    ZIndex = 5,
                }, track)
                corner(3, fill)
                gradient(T.accent2, T.accent3, 0, fill)

                local knob = makeInst("Frame", {
                    Size = UDim2.new(0, 12, 0, 12),
                    Position = UDim2.new((default - min) / (max - min), -6, 0.5, -6),
                    BackgroundColor3 = T.white,
                    BorderSizePixel = 0,
                    ZIndex = 6,
                }, track)
                corner(6, knob)
                stroke(T.accent3, 1.5, knob)

                local function setValue(pct)
                    pct = math.clamp(pct, 0, 1)
                    local v = math.floor(min + (max - min) * pct)
                    if opts.interval then
                        v = math.floor(v / opts.interval + 0.5) * opts.interval
                    end
                    dalol.flags[flag] = v
                    fill.Size = UDim2.new(pct, 0, 1, 0)
                    knob.Position = UDim2.new(pct, -6, 0.5, -6)
                    valLbl.Text = tostring(v) .. suffix
                    if cb then pcall(cb, v) end
                end

                dalol.callbacks[flag] = function(v)
                    setValue((v - min) / (max - min))
                end

                local sliding = false
                track.InputBegan:Connect(function(input)
                    if input.UserInputType == Enum.UserInputType.MouseButton1 then
                        sliding = true
                        tween(knob, {Size = UDim2.new(0, 14, 0, 14), Position = UDim2.new(knob.Position.X.Scale, -7, 0.5, -7)}, 0.1, Enum.EasingStyle.Back)
                    end
                end)
                UserInputService.InputEnded:Connect(function(input)
                    if input.UserInputType == Enum.UserInputType.MouseButton1 then
                        if sliding then
                            tween(knob, {Size = UDim2.new(0, 12, 0, 12)}, 0.1)
                            sliding = false
                        end
                    end
                end)
                UserInputService.InputChanged:Connect(function(input)
                    if sliding and input.UserInputType == Enum.UserInputType.MouseMovement then
                        local abs = track.AbsolutePosition
                        local sz  = track.AbsoluteSize
                        local pct = (input.Position.X - abs.X) / sz.X
                        setValue(pct)
                    end
                end)
            end

            -- ── DROPDOWN ──
            function secObj:dropdown(opts)
                opts = opts or {}
                local flag    = opts.flag or opts.name
                local list_   = opts.list or {}
                local default = opts.default or list_[1] or ""
                local cb      = opts.callback

                dalol.flags[flag] = default

                local container = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 0),
                    BackgroundTransparency = 1,
                    AutomaticSize = Enum.AutomaticSize.Y,
                    ZIndex = 10,
                }, body)
                list(4, container)

                local row = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 28),
                    BackgroundTransparency = 1,
                    ZIndex = 10,
                }, container)
                makeLabel(row, opts.name or "Dropdown")

                -- selected display
                local selFrame = makeInst("Frame", {
                    Size = UDim2.new(0.44, 0, 0, 24),
                    Position = UDim2.new(0.56, 0, 0.5, -12),
                    BackgroundColor3 = T.bg3,
                    BorderSizePixel = 0,
                    ZIndex = 11,
                }, row)
                corner(7, selFrame)
                stroke(T.border2, 1, selFrame)

                local selLbl = makeInst("TextLabel", {
                    Size = UDim2.new(1, -20, 1, 0),
                    Position = UDim2.new(0, 8, 0, 0),
                    BackgroundTransparency = 1,
                    Text = default,
                    TextColor3 = T.text,
                    Font = Enum.Font.Gotham,
                    TextSize = 12,
                    TextXAlignment = Enum.TextXAlignment.Left,
                    ZIndex = 12,
                }, selFrame)

                local arrow = makeInst("TextLabel", {
                    Size = UDim2.new(0, 16, 1, 0),
                    Position = UDim2.new(1, -18, 0, 0),
                    BackgroundTransparency = 1,
                    Text = "▼",
                    TextColor3 = T.accent,
                    Font = Enum.Font.GothamBold,
                    TextSize = 10,
                    ZIndex = 12,
                }, selFrame)

                -- options panel
                local optFrame = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 0),
                    BackgroundColor3 = T.bg3,
                    BorderSizePixel = 0,
                    ClipsDescendants = true,
                    ZIndex = 20,
                }, container)
                corner(7, optFrame)
                stroke(Color3.fromHex("3a1f7a"), 1, optFrame)

                local optList = list(0, optFrame)

                local isOpen = false
                local totalH = 0

                for _, item in ipairs(list_) do
                    local optBtn = makeInst("TextButton", {
                        Size = UDim2.new(1, 0, 0, 26),
                        BackgroundColor3 = T.bg3,
                        BorderSizePixel = 0,
                        Text = "",
                        ZIndex = 21,
                    }, optFrame)
                    makeInst("TextLabel", {
                        Size = UDim2.new(1, -10, 1, 0),
                        Position = UDim2.new(0, 10, 0, 0),
                        BackgroundTransparency = 1,
                        Text = "▸ " .. item,
                        TextColor3 = item == default and T.accent3 or T.text3,
                        Font = Enum.Font.Gotham,
                        TextSize = 12,
                        TextXAlignment = Enum.TextXAlignment.Left,
                        ZIndex = 22,
                    }, optBtn)
                    makeInst("Frame", {
                        Size = UDim2.new(1, 0, 0, 1),
                        Position = UDim2.new(0, 0, 1, -1),
                        BackgroundColor3 = T.border,
                        BorderSizePixel = 0,
                        ZIndex = 22,
                    }, optBtn)

                    totalH = totalH + 26

                    optBtn.MouseButton1Click:Connect(function()
                        dalol.flags[flag] = item
                        selLbl.Text = item
                        -- update colors
                        for _, c in ipairs(optFrame:GetChildren()) do
                            if c:IsA("TextButton") then
                                local lbl = c:FindFirstChildOfClass("TextLabel")
                                if lbl then
                                    lbl.TextColor3 = lbl.Text:find(item, 1, true) and T.accent3 or T.text3
                                end
                            end
                        end
                        if cb then pcall(cb, item) end
                        -- close
                        isOpen = false
                        tween(optFrame, {Size = UDim2.new(1, 0, 0, 0)}, 0.2)
                        tween(arrow, {Rotation = 0}, 0.2)
                    end)
                end

                local ddBtn = makeInst("TextButton", {
                    Size = UDim2.new(1, 0, 1, 0),
                    BackgroundTransparency = 1,
                    Text = "",
                    ZIndex = 13,
                }, selFrame)
                ddBtn.MouseButton1Click:Connect(function()
                    isOpen = not isOpen
                    if isOpen then
                        tween(optFrame, {Size = UDim2.new(1, 0, 0, totalH)}, 0.25, Enum.EasingStyle.Back)
                        tween(arrow, {Rotation = 180}, 0.2)
                    else
                        tween(optFrame, {Size = UDim2.new(1, 0, 0, 0)}, 0.2)
                        tween(arrow, {Rotation = 0}, 0.2)
                    end
                end)
            end

            -- ── COLORPICKER ──
            function secObj:colorpicker(opts)
                opts = opts or {}
                local flag  = opts.flag or opts.name
                local color = opts.color or Color3.fromRGB(255,255,255)
                local cb    = opts.callback

                dalol.flags[flag] = color

                local row = makeRow()
                makeLabel(row, opts.name or "Color")

                local swatchHolder = makeInst("Frame", {
                    Size = UDim2.new(0, 24, 0, 24),
                    Position = UDim2.new(1, -70, 0.5, -12),
                    BackgroundColor3 = color,
                    BorderSizePixel = 0,
                    ZIndex = 4,
                }, row)
                corner(6, swatchHolder)
                stroke(T.border2, 1, swatchHolder)

                local hexLbl = makeInst("TextLabel", {
                    Size = UDim2.new(0, 42, 0, 24),
                    Position = UDim2.new(1, -44, 0.5, -12),
                    BackgroundTransparency = 1,
                    Text = "#" .. string.format("%02X%02X%02X",
                        math.floor(color.R*255),
                        math.floor(color.G*255),
                        math.floor(color.B*255)),
                    TextColor3 = T.text3,
                    Font = Enum.Font.Code,
                    TextSize = 9,
                    ZIndex = 4,
                }, row)

                -- Simple color picker popup
                local pickerOpen = false
                local pickerFrame

                local swBtn = makeInst("TextButton", {
                    Size = UDim2.new(1, 0, 1, 0),
                    BackgroundTransparency = 1,
                    Text = "",
                    ZIndex = 5,
                }, swatchHolder)

                swBtn.MouseButton1Click:Connect(function()
                    pickerOpen = not pickerOpen
                    if pickerFrame then pickerFrame:Destroy() pickerFrame = nil end
                    if not pickerOpen then return end

                    pickerFrame = makeInst("Frame", {
                        Size = UDim2.new(0, 180, 0, 130),
                        Position = UDim2.new(0, 0, 1, 4),
                        BackgroundColor3 = T.bg3,
                        BorderSizePixel = 0,
                        ZIndex = 50,
                    }, swatchHolder)
                    corner(8, pickerFrame)
                    stroke(T.border2, 1, pickerFrame)
                    padding(8, 8, 8, 8, pickerFrame)
                    list(6, pickerFrame)

                    makeInst("TextLabel", {
                        Size = UDim2.new(1, 0, 0, 14),
                        BackgroundTransparency = 1,
                        Text = "R  G  B  SLIDERS",
                        TextColor3 = T.text3,
                        Font = Enum.Font.Code,
                        TextSize = 9,
                        ZIndex = 51,
                    }, pickerFrame)

                    local r, g, b = color.R, color.G, color.B
                    local channels = {{"R", "ff4444"}, {"G", "44ff88"}, {"B", "4488ff"}}
                    local vals = {r, g, b}

                    local function updateColor()
                        local nc = Color3.new(vals[1], vals[2], vals[3])
                        dalol.flags[flag] = nc
                        swatchHolder.BackgroundColor3 = nc
                        hexLbl.Text = "#" .. string.format("%02X%02X%02X",
                            math.floor(nc.R*255), math.floor(nc.G*255), math.floor(nc.B*255))
                        if cb then pcall(cb, nc) end
                    end

                    for i, ch in ipairs(channels) do
                        local chRow = makeInst("Frame", {
                            Size = UDim2.new(1, 0, 0, 22),
                            BackgroundTransparency = 1,
                            ZIndex = 51,
                        }, pickerFrame)

                        makeInst("TextLabel", {
                            Size = UDim2.new(0, 12, 1, 0),
                            BackgroundTransparency = 1,
                            Text = ch[1],
                            TextColor3 = Color3.fromHex(ch[2]),
                            Font = Enum.Font.Code,
                            TextSize = 11,
                            ZIndex = 52,
                        }, chRow)

                        local ct = makeInst("Frame", {
                            Size = UDim2.new(1, -40, 0, 4),
                            Position = UDim2.new(0, 16, 0.5, -2),
                            BackgroundColor3 = Color3.fromHex("181530"),
                            BorderSizePixel = 0,
                            ZIndex = 52,
                        }, chRow)
                        corner(2, ct)

                        local cf2 = makeInst("Frame", {
                            Size = UDim2.new(vals[i], 0, 1, 0),
                            BackgroundColor3 = Color3.fromHex(ch[2]),
                            BorderSizePixel = 0,
                            ZIndex = 53,
                        }, ct)
                        corner(2, cf2)

                        local ck = makeInst("Frame", {
                            Size = UDim2.new(0, 10, 0, 10),
                            Position = UDim2.new(vals[i], -5, 0.5, -5),
                            BackgroundColor3 = T.white,
                            BorderSizePixel = 0,
                            ZIndex = 54,
                        }, ct)
                        corner(5, ck)

                        local valLbl2 = makeInst("TextLabel", {
                            Size = UDim2.new(0, 22, 1, 0),
                            Position = UDim2.new(1, -22, 0, 0),
                            BackgroundTransparency = 1,
                            Text = tostring(math.floor(vals[i]*255)),
                            TextColor3 = Color3.fromHex(ch[2]),
                            Font = Enum.Font.Code,
                            TextSize = 9,
                            TextXAlignment = Enum.TextXAlignment.Right,
                            ZIndex = 52,
                        }, chRow)

                        local sliding2 = false
                        ct.InputBegan:Connect(function(inp)
                            if inp.UserInputType == Enum.UserInputType.MouseButton1 then
                                sliding2 = true
                            end
                        end)
                        UserInputService.InputEnded:Connect(function(inp)
                            if inp.UserInputType == Enum.UserInputType.MouseButton1 then
                                sliding2 = false
                            end
                        end)
                        UserInputService.InputChanged:Connect(function(inp)
                            if sliding2 and inp.UserInputType == Enum.UserInputType.MouseMovement then
                                local p = math.clamp((inp.Position.X - ct.AbsolutePosition.X) / ct.AbsoluteSize.X, 0, 1)
                                vals[i] = p
                                cf2.Size = UDim2.new(p, 0, 1, 0)
                                ck.Position = UDim2.new(p, -5, 0.5, -5)
                                valLbl2.Text = tostring(math.floor(p*255))
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

                local btn = makeInst("TextButton", {
                    Size = UDim2.new(1, 0, 0, 30),
                    BackgroundColor3 = Color3.fromHex("3a1580"),
                    BorderSizePixel = 0,
                    Text = opts.name or "Button",
                    TextColor3 = T.text2,
                    Font = Enum.Font.Code,
                    TextSize = 11,
                    ZIndex = 4,
                }, body)
                corner(8, btn)
                stroke(Color3.fromHex("7b2fff55"), 1, btn)
                gradient(Color3.fromHex("3a1580"), Color3.fromHex("5b2db0"), 135, btn)

                btn.MouseEnter:Connect(function()
                    tween(btn, {BackgroundColor3 = Color3.fromHex("4a1fa0")}, 0.15)
                end)
                btn.MouseLeave:Connect(function()
                    tween(btn, {BackgroundColor3 = Color3.fromHex("3a1580")}, 0.15)
                end)
                btn.MouseButton1Down:Connect(function()
                    tween(btn, {Size = UDim2.new(1, 0, 0, 26)}, 0.08, Enum.EasingStyle.Quart)
                end)
                btn.MouseButton1Up:Connect(function()
                    tween(btn, {Size = UDim2.new(1, 0, 0, 30)}, 0.15, Enum.EasingStyle.Back)
                    if cb then pcall(cb) end
                end)
            end

            -- ── KEYBIND ──
            function secObj:keybind(opts)
                opts = opts or {}
                local flag    = opts.flag or opts.name
                local default = opts.default or Enum.KeyCode.Unknown
                local cb      = opts.callback
                local listening = false

                dalol.flags[flag] = default

                local row = makeRow()
                makeLabel(row, opts.name or "Keybind")

                local kbFrame = makeInst("Frame", {
                    Size = UDim2.new(0.43, 0, 0, 22),
                    Position = UDim2.new(0.57, 0, 0.5, -11),
                    BackgroundColor3 = T.bg3,
                    BorderSizePixel = 0,
                    ZIndex = 4,
                }, row)
                corner(5, kbFrame)
                stroke(T.border2, 1, kbFrame)
                -- inset shadow
                makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 2),
                    Position = UDim2.new(0, 0, 1, -2),
                    BackgroundColor3 = Color3.fromHex("1a0d40"),
                    BorderSizePixel = 0,
                    ZIndex = 4,
                }, kbFrame)

                local kbLbl = makeInst("TextLabel", {
                    Size = UDim2.new(1, 0, 1, 0),
                    BackgroundTransparency = 1,
                    Text = default.Name:upper(),
                    TextColor3 = T.secTitle,
                    Font = Enum.Font.Code,
                    TextSize = 10,
                    ZIndex = 5,
                }, kbFrame)

                local kbBtn = makeInst("TextButton", {
                    Size = UDim2.new(1, 0, 1, 0),
                    BackgroundTransparency = 1,
                    Text = "",
                    ZIndex = 6,
                }, kbFrame)

                kbBtn.MouseButton1Click:Connect(function()
                    listening = true
                    kbLbl.Text = "..."
                    tween(kbFrame, {BackgroundColor3 = Color3.fromHex("1a1040")}, 0.1)
                end)

                UserInputService.InputBegan:Connect(function(input, gp)
                    if not listening then
                        -- execute keybind
                        if not gp and input.KeyCode == dalol.flags[flag] then
                            if cb then pcall(cb) end
                        end
                        return
                    end
                    if input.UserInputType == Enum.UserInputType.Keyboard then
                        listening = false
                        dalol.flags[flag] = input.KeyCode
                        kbLbl.Text = input.KeyCode.Name:upper()
                        tween(kbFrame, {BackgroundColor3 = T.bg3}, 0.1)
                    end
                end)
            end

            -- ── LABEL ──
            function secObj:label(opts)
                opts = opts or {}
                local text = opts.text or opts.name or ""

                local row = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 18),
                    BackgroundTransparency = 1,
                    ZIndex = 3,
                }, body)

                local lineL = makeInst("Frame", {
                    Size = UDim2.new(0.5, -50, 0, 1),
                    Position = UDim2.new(0, 0, 0.5, 0),
                    BackgroundColor3 = T.border2,
                    BorderSizePixel = 0,
                    ZIndex = 4,
                }, row)
                gradient(Color3.fromHex("251a4a"), T.bg2, 0, lineL)

                makeInst("TextLabel", {
                    Size = UDim2.new(0, 100, 1, 0),
                    Position = UDim2.new(0.5, -50, 0, 0),
                    BackgroundTransparency = 1,
                    Text = text:upper(),
                    TextColor3 = T.text3,
                    Font = Enum.Font.Code,
                    TextSize = 9,
                    ZIndex = 4,
                }, row)

                local lineR = makeInst("Frame", {
                    Size = UDim2.new(0.5, -50, 0, 1),
                    Position = UDim2.new(0.5, 50, 0.5, 0),
                    BackgroundColor3 = T.border2,
                    BorderSizePixel = 0,
                    ZIndex = 4,
                }, row)
                gradient(T.bg2, Color3.fromHex("251a4a"), 0, lineR)
            end

            -- ── TEXTBOX ──
            function secObj:textbox(opts)
                opts = opts or {}
                local flag    = opts.flag or opts.name
                local default = opts.default or ""
                local cb      = opts.callback

                dalol.flags[flag] = default

                local row = makeRow()
                makeLabel(row, opts.name or "Input")

                local tbFrame = makeInst("Frame", {
                    Size = UDim2.new(0.44, 0, 0, 24),
                    Position = UDim2.new(0.56, 0, 0.5, -12),
                    BackgroundColor3 = T.bg3,
                    BorderSizePixel = 0,
                    ZIndex = 4,
                }, row)
                corner(7, tbFrame)
                stroke(T.border2, 1, tbFrame)

                local tb2 = makeInst("TextBox", {
                    Size = UDim2.new(1, -8, 1, 0),
                    Position = UDim2.new(0, 6, 0, 0),
                    BackgroundTransparency = 1,
                    Text = default,
                    PlaceholderText = opts.placeholder or "type...",
                    TextColor3 = T.text,
                    PlaceholderColor3 = T.text3,
                    Font = Enum.Font.Gotham,
                    TextSize = 12,
                    TextXAlignment = Enum.TextXAlignment.Left,
                    ClearTextOnFocus = opts.clearOnFocus ~= false,
                    ZIndex = 5,
                }, tbFrame)

                tb2.Focused:Connect(function()
                    tween(tbFrame, {BackgroundColor3 = Color3.fromHex("13102a")}, 0.15)
                    stroke(T.accent, 1, tbFrame)
                end)
                tb2.FocusLost:Connect(function(enter)
                    tween(tbFrame, {BackgroundColor3 = T.bg3}, 0.15)
                    dalol.flags[flag] = tb2.Text
                    if cb then pcall(cb, tb2.Text, enter) end
                end)
            end

            -- ── DIVIDER ──
            function secObj:divider()
                local div = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 1),
                    BackgroundColor3 = T.border2,
                    BorderSizePixel = 0,
                    ZIndex = 3,
                }, body)
                gradient(T.bg2, Color3.fromHex("251a4a"), 0, div)
            end

            return secObj
        end

        return colObj
    end

    -- ── CONFIG SECTION builder ──
    function tabObj:configSection()
        local col2 = self:column()
        local sec  = col2:section({name = "Config"})

        sec:label({text = "SAVED CONFIGS"})

        local cfgListFrame = makeInst("Frame", {
            Size = UDim2.new(1, 0, 0, 0),
            BackgroundTransparency = 1,
            AutomaticSize = Enum.AutomaticSize.Y,
            ZIndex = 4,
        }, sec._body or body)

        local function refreshCfgList()
            for _, c in ipairs(cfgListFrame:GetChildren()) do
                if not c:IsA("UIListLayout") then c:Destroy() end
            end
            for _, name_ in ipairs(dalol:listConfigs()) do
                local item = makeInst("Frame", {
                    Size = UDim2.new(1, 0, 0, 26),
                    BackgroundColor3 = Color3.fromHex("13102a"),
                    BorderSizePixel = 0,
                    ZIndex = 5,
                }, cfgListFrame)
                corner(6, item)

                makeInst("TextLabel", {
                    Size = UDim2.new(1, -50, 1, 0),
                    Position = UDim2.new(0, 8, 0, 0),
                    BackgroundTransparency = 1,
                    Text = name_,
                    TextColor3 = T.text,
                    Font = Enum.Font.Gotham,
                    TextSize = 12,
                    TextXAlignment = Enum.TextXAlignment.Left,
                    ZIndex = 6,
                }, item)

                local loadBtn = makeInst("TextButton", {
                    Size = UDim2.new(0, 20, 0, 20),
                    Position = UDim2.new(1, -44, 0.5, -10),
                    BackgroundTransparency = 1,
                    Text = "↑",
                    TextColor3 = T.secTitle,
                    Font = Enum.Font.GothamBold,
                    TextSize = 14,
                    ZIndex = 7,
                }, item)
                loadBtn.MouseButton1Click:Connect(function()
                    dalol:loadConfig(name_)
                end)

                local delBtn = makeInst("TextButton", {
                    Size = UDim2.new(0, 20, 0, 20),
                    Position = UDim2.new(1, -22, 0.5, -10),
                    BackgroundTransparency = 1,
                    Text = "✕",
                    TextColor3 = T.text3,
                    Font = Enum.Font.GothamBold,
                    TextSize = 12,
                    ZIndex = 7,
                }, item)
                delBtn.MouseButton1Click:Connect(function()
                    local path = CONFIG_PATH .. name_ .. ".json"
                    if isfile(path) then delfile(path) end
                    refreshCfgList()
                end)
            end
        end

        refreshCfgList()

        sec:button({name = "SAVE CONFIG", callback = function()
            dalol:saveConfig("default")
            refreshCfgList()
        end})
        sec:button({name = "LOAD CONFIG", callback = function()
            dalol:loadConfig("default")
        end})

        sec:textbox({name = "Config Name", flag = "cfg_name", placeholder = "config name...",
            callback = function(v)
                dalol.flags["cfg_name"] = v
            end
        })
    end

    return tabObj
end

function winObj:notify(t, m, i, d)
    dalol:notify(t, m, i, d)
end

function winObj:setWatermark(t)
    dalol:setWatermark(t)
end

return winObj
end

return dalol
