<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class FormMain
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(FormMain))
        Me.btnClose = New System.Windows.Forms.Button
        Me.lblPreconfigured = New System.Windows.Forms.Label
        Me.lblFramework = New System.Windows.Forms.Label
        Me.txtFramework = New System.Windows.Forms.TextBox
        Me.btnFrameworkSelect = New System.Windows.Forms.Button
        Me.btnWorkbooks = New System.Windows.Forms.Button
        Me.txtWorkbooks = New System.Windows.Forms.TextBox
        Me.lblWorkbooks = New System.Windows.Forms.Label
        Me.grpEnvironment = New System.Windows.Forms.GroupBox
        Me.tstEnvironment = New System.Windows.Forms.ToolStrip
        Me.btnNew = New System.Windows.Forms.ToolStripButton
        Me.btnCopy = New System.Windows.Forms.ToolStripButton
        Me.btnDelete = New System.Windows.Forms.ToolStripButton
        Me.btnClear = New System.Windows.Forms.ToolStripButton
        Me.btnRename = New System.Windows.Forms.ToolStripButton
        Me.lstUserconfigured = New System.Windows.Forms.ListBox
        Me.lblUserConfigured = New System.Windows.Forms.Label
        Me.lstPreconfigured = New System.Windows.Forms.ListBox
        Me.grpStartup = New System.Windows.Forms.GroupBox
        Me.cbStaticData = New System.Windows.Forms.CheckBox
        Me.cbFitCMS = New System.Windows.Forms.CheckBox
        Me.cbLoadBonds = New System.Windows.Forms.CheckBox
        Me.cbSwapSmileBootstrap = New System.Windows.Forms.CheckBox
        Me.cbIndexesTimeSeries = New System.Windows.Forms.CheckBox
        Me.cbCapVolBootstrap = New System.Windows.Forms.CheckBox
        Me.cbLoadMurexYC = New System.Windows.Forms.CheckBox
        Me.cbYCBootstrap = New System.Windows.Forms.CheckBox
        Me.btnLaunch = New System.Windows.Forms.Button
        Me.tcLauncher = New System.Windows.Forms.TabControl
        Me.tpEnvironments = New System.Windows.Forms.TabPage
        Me.tpPaths = New System.Windows.Forms.TabPage
        Me.lblFrameworkVersion = New System.Windows.Forms.Label
        Me.mskFrameworkVersion = New System.Windows.Forms.MaskedTextBox
        Me.txtUserConfig = New System.Windows.Forms.TextBox
        Me.lblUserConfig = New System.Windows.Forms.Label
        Me.btnUserConfig = New System.Windows.Forms.Button
        Me.txtXmlPath = New System.Windows.Forms.TextBox
        Me.lblXmlPath = New System.Windows.Forms.Label
        Me.btnXmlPath = New System.Windows.Forms.Button
        Me.txtHelpPath = New System.Windows.Forms.TextBox
        Me.lblHelpPath = New System.Windows.Forms.Label
        Me.btnHelpFile = New System.Windows.Forms.Button
        Me.tpAddins = New System.Windows.Forms.TabPage
        Me.lbAddins = New System.Windows.Forms.ListBox
        Me.tstAddins = New System.Windows.Forms.ToolStrip
        Me.btnAddinInsert = New System.Windows.Forms.ToolStripButton
        Me.btnAddinDelete = New System.Windows.Forms.ToolStripButton
        Me.btnAddinRename = New System.Windows.Forms.ToolStripButton
        Me.btnAddinUp = New System.Windows.Forms.ToolStripButton
        Me.btnAddinDown = New System.Windows.Forms.ToolStripButton
        Me.grpEnvironment.SuspendLayout()
        Me.tstEnvironment.SuspendLayout()
        Me.grpStartup.SuspendLayout()
        Me.tcLauncher.SuspendLayout()
        Me.tpEnvironments.SuspendLayout()
        Me.tpPaths.SuspendLayout()
        Me.tpAddins.SuspendLayout()
        Me.tstAddins.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnClose
        '
        Me.btnClose.Location = New System.Drawing.Point(207, 345)
        Me.btnClose.Name = "btnClose"
        Me.btnClose.Size = New System.Drawing.Size(75, 23)
        Me.btnClose.TabIndex = 1
        Me.btnClose.Text = "Close"
        Me.btnClose.UseVisualStyleBackColor = True
        '
        'lblPreconfigured
        '
        Me.lblPreconfigured.AutoSize = True
        Me.lblPreconfigured.Location = New System.Drawing.Point(6, 22)
        Me.lblPreconfigured.Name = "lblPreconfigured"
        Me.lblPreconfigured.Size = New System.Drawing.Size(73, 13)
        Me.lblPreconfigured.TabIndex = 3
        Me.lblPreconfigured.Text = "Preconfigured"
        '
        'lblFramework
        '
        Me.lblFramework.AutoSize = True
        Me.lblFramework.Location = New System.Drawing.Point(10, 10)
        Me.lblFramework.Name = "lblFramework"
        Me.lblFramework.Size = New System.Drawing.Size(59, 13)
        Me.lblFramework.TabIndex = 4
        Me.lblFramework.Text = "Framework"
        '
        'txtFramework
        '
        Me.txtFramework.Location = New System.Drawing.Point(30, 26)
        Me.txtFramework.Name = "txtFramework"
        Me.txtFramework.Size = New System.Drawing.Size(315, 20)
        Me.txtFramework.TabIndex = 5
        '
        'btnFrameworkSelect
        '
        Me.btnFrameworkSelect.Location = New System.Drawing.Point(351, 26)
        Me.btnFrameworkSelect.Name = "btnFrameworkSelect"
        Me.btnFrameworkSelect.Size = New System.Drawing.Size(32, 23)
        Me.btnFrameworkSelect.TabIndex = 11
        Me.btnFrameworkSelect.Text = "..."
        Me.btnFrameworkSelect.UseVisualStyleBackColor = True
        '
        'btnWorkbooks
        '
        Me.btnWorkbooks.Location = New System.Drawing.Point(351, 104)
        Me.btnWorkbooks.Name = "btnWorkbooks"
        Me.btnWorkbooks.Size = New System.Drawing.Size(32, 23)
        Me.btnWorkbooks.TabIndex = 15
        Me.btnWorkbooks.Text = "..."
        Me.btnWorkbooks.UseVisualStyleBackColor = True
        '
        'txtWorkbooks
        '
        Me.txtWorkbooks.Location = New System.Drawing.Point(30, 104)
        Me.txtWorkbooks.Name = "txtWorkbooks"
        Me.txtWorkbooks.Size = New System.Drawing.Size(315, 20)
        Me.txtWorkbooks.TabIndex = 14
        '
        'lblWorkbooks
        '
        Me.lblWorkbooks.AutoSize = True
        Me.lblWorkbooks.Location = New System.Drawing.Point(10, 88)
        Me.lblWorkbooks.Name = "lblWorkbooks"
        Me.lblWorkbooks.Size = New System.Drawing.Size(62, 13)
        Me.lblWorkbooks.TabIndex = 13
        Me.lblWorkbooks.Text = "Workbooks"
        '
        'grpEnvironment
        '
        Me.grpEnvironment.Controls.Add(Me.tstEnvironment)
        Me.grpEnvironment.Controls.Add(Me.lstUserconfigured)
        Me.grpEnvironment.Controls.Add(Me.lblUserConfigured)
        Me.grpEnvironment.Controls.Add(Me.lstPreconfigured)
        Me.grpEnvironment.Controls.Add(Me.lblPreconfigured)
        Me.grpEnvironment.Location = New System.Drawing.Point(6, 7)
        Me.grpEnvironment.Name = "grpEnvironment"
        Me.grpEnvironment.Size = New System.Drawing.Size(190, 300)
        Me.grpEnvironment.TabIndex = 16
        Me.grpEnvironment.TabStop = False
        Me.grpEnvironment.Text = "Environments"
        '
        'tstEnvironment
        '
        Me.tstEnvironment.BackColor = System.Drawing.SystemColors.Control
        Me.tstEnvironment.Dock = System.Windows.Forms.DockStyle.None
        Me.tstEnvironment.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden
        Me.tstEnvironment.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.btnNew, Me.btnCopy, Me.btnDelete, Me.btnClear, Me.btnRename})
        Me.tstEnvironment.Location = New System.Drawing.Point(9, 267)
        Me.tstEnvironment.Name = "tstEnvironment"
        Me.tstEnvironment.RenderMode = System.Windows.Forms.ToolStripRenderMode.System
        Me.tstEnvironment.Size = New System.Drawing.Size(118, 25)
        Me.tstEnvironment.TabIndex = 7
        Me.tstEnvironment.Text = "ToolStrip1"
        '
        'btnNew
        '
        Me.btnNew.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnNew.Image = Global.launcher.My.Resources.Resources.create
        Me.btnNew.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnNew.Name = "btnNew"
        Me.btnNew.Size = New System.Drawing.Size(23, 22)
        Me.btnNew.Text = "New"
        '
        'btnCopy
        '
        Me.btnCopy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnCopy.Image = Global.launcher.My.Resources.Resources.copy
        Me.btnCopy.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnCopy.Name = "btnCopy"
        Me.btnCopy.Size = New System.Drawing.Size(23, 22)
        Me.btnCopy.Text = "Copy"
        '
        'btnDelete
        '
        Me.btnDelete.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnDelete.Image = Global.launcher.My.Resources.Resources.delete
        Me.btnDelete.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnDelete.Name = "btnDelete"
        Me.btnDelete.Size = New System.Drawing.Size(23, 22)
        Me.btnDelete.Text = "Delete"
        '
        'btnClear
        '
        Me.btnClear.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnClear.Image = Global.launcher.My.Resources.Resources.clear
        Me.btnClear.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnClear.Name = "btnClear"
        Me.btnClear.Size = New System.Drawing.Size(23, 22)
        Me.btnClear.Text = "Clear"
        '
        'btnRename
        '
        Me.btnRename.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnRename.Image = Global.launcher.My.Resources.Resources.rename
        Me.btnRename.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnRename.Name = "btnRename"
        Me.btnRename.Size = New System.Drawing.Size(23, 22)
        Me.btnRename.Text = "Rename"
        '
        'lstUserconfigured
        '
        Me.lstUserconfigured.FormattingEnabled = True
        Me.lstUserconfigured.Location = New System.Drawing.Point(19, 126)
        Me.lstUserconfigured.Name = "lstUserconfigured"
        Me.lstUserconfigured.Size = New System.Drawing.Size(160, 134)
        Me.lstUserconfigured.Sorted = True
        Me.lstUserconfigured.TabIndex = 6
        '
        'lblUserConfigured
        '
        Me.lblUserConfigured.AutoSize = True
        Me.lblUserConfigured.Location = New System.Drawing.Point(6, 110)
        Me.lblUserConfigured.Name = "lblUserConfigured"
        Me.lblUserConfigured.Size = New System.Drawing.Size(83, 13)
        Me.lblUserConfigured.TabIndex = 5
        Me.lblUserConfigured.Text = "User Configured"
        '
        'lstPreconfigured
        '
        Me.lstPreconfigured.FormattingEnabled = True
        Me.lstPreconfigured.Location = New System.Drawing.Point(19, 38)
        Me.lstPreconfigured.Name = "lstPreconfigured"
        Me.lstPreconfigured.Size = New System.Drawing.Size(160, 69)
        Me.lstPreconfigured.Sorted = True
        Me.lstPreconfigured.TabIndex = 4
        '
        'grpStartup
        '
        Me.grpStartup.Controls.Add(Me.cbStaticData)
        Me.grpStartup.Controls.Add(Me.cbFitCMS)
        Me.grpStartup.Controls.Add(Me.cbLoadBonds)
        Me.grpStartup.Controls.Add(Me.cbSwapSmileBootstrap)
        Me.grpStartup.Controls.Add(Me.cbIndexesTimeSeries)
        Me.grpStartup.Controls.Add(Me.cbCapVolBootstrap)
        Me.grpStartup.Controls.Add(Me.cbLoadMurexYC)
        Me.grpStartup.Controls.Add(Me.cbYCBootstrap)
        Me.grpStartup.Location = New System.Drawing.Point(202, 6)
        Me.grpStartup.Name = "grpStartup"
        Me.grpStartup.Size = New System.Drawing.Size(190, 301)
        Me.grpStartup.TabIndex = 19
        Me.grpStartup.TabStop = False
        Me.grpStartup.Text = "Startup Actions"
        '
        'cbStaticData
        '
        Me.cbStaticData.AutoSize = True
        Me.cbStaticData.Location = New System.Drawing.Point(10, 187)
        Me.cbStaticData.Name = "cbStaticData"
        Me.cbStaticData.Size = New System.Drawing.Size(159, 17)
        Me.cbStaticData.TabIndex = 8
        Me.cbStaticData.Text = "Default to static market data"
        Me.cbStaticData.UseVisualStyleBackColor = True
        '
        'cbFitCMS
        '
        Me.cbFitCMS.AutoSize = True
        Me.cbFitCMS.Location = New System.Drawing.Point(10, 118)
        Me.cbFitCMS.Name = "cbFitCMS"
        Me.cbFitCMS.Size = New System.Drawing.Size(111, 17)
        Me.cbFitCMS.TabIndex = 7
        Me.cbFitCMS.Text = "View CMS Market"
        Me.cbFitCMS.UseVisualStyleBackColor = True
        '
        'cbLoadBonds
        '
        Me.cbLoadBonds.AutoSize = True
        Me.cbLoadBonds.Location = New System.Drawing.Point(10, 164)
        Me.cbLoadBonds.Name = "cbLoadBonds"
        Me.cbLoadBonds.Size = New System.Drawing.Size(83, 17)
        Me.cbLoadBonds.TabIndex = 6
        Me.cbLoadBonds.Text = "Load Bonds"
        Me.cbLoadBonds.UseVisualStyleBackColor = True
        '
        'cbSwapSmileBootstrap
        '
        Me.cbSwapSmileBootstrap.AutoSize = True
        Me.cbSwapSmileBootstrap.Location = New System.Drawing.Point(10, 95)
        Me.cbSwapSmileBootstrap.Name = "cbSwapSmileBootstrap"
        Me.cbSwapSmileBootstrap.Size = New System.Drawing.Size(173, 17)
        Me.cbSwapSmileBootstrap.TabIndex = 5
        Me.cbSwapSmileBootstrap.Text = "Create Swaption Smile Volatility"
        Me.cbSwapSmileBootstrap.UseVisualStyleBackColor = True
        '
        'cbIndexesTimeSeries
        '
        Me.cbIndexesTimeSeries.AutoSize = True
        Me.cbIndexesTimeSeries.Location = New System.Drawing.Point(10, 141)
        Me.cbIndexesTimeSeries.Name = "cbIndexesTimeSeries"
        Me.cbIndexesTimeSeries.Size = New System.Drawing.Size(148, 17)
        Me.cbIndexesTimeSeries.TabIndex = 4
        Me.cbIndexesTimeSeries.Text = "Load Indexes Time Series"
        Me.cbIndexesTimeSeries.UseVisualStyleBackColor = True
        '
        'cbCapVolBootstrap
        '
        Me.cbCapVolBootstrap.AutoSize = True
        Me.cbCapVolBootstrap.Location = New System.Drawing.Point(10, 72)
        Me.cbCapVolBootstrap.Name = "cbCapVolBootstrap"
        Me.cbCapVolBootstrap.Size = New System.Drawing.Size(142, 17)
        Me.cbCapVolBootstrap.TabIndex = 2
        Me.cbCapVolBootstrap.Text = "Bootstrap Cap Volatilities"
        Me.cbCapVolBootstrap.UseVisualStyleBackColor = True
        '
        'cbLoadMurexYC
        '
        Me.cbLoadMurexYC.AutoSize = True
        Me.cbLoadMurexYC.Location = New System.Drawing.Point(10, 47)
        Me.cbLoadMurexYC.Name = "cbLoadMurexYC"
        Me.cbLoadMurexYC.Size = New System.Drawing.Size(160, 17)
        Me.cbLoadMurexYC.TabIndex = 1
        Me.cbLoadMurexYC.Text = "Bootstrap Murex Yield Curve"
        Me.cbLoadMurexYC.UseVisualStyleBackColor = True
        '
        'cbYCBootstrap
        '
        Me.cbYCBootstrap.AutoSize = True
        Me.cbYCBootstrap.Location = New System.Drawing.Point(10, 23)
        Me.cbYCBootstrap.Name = "cbYCBootstrap"
        Me.cbYCBootstrap.Size = New System.Drawing.Size(174, 17)
        Me.cbYCBootstrap.TabIndex = 0
        Me.cbYCBootstrap.Text = "Bootstrap QuantLib Yield Curve"
        Me.cbYCBootstrap.UseVisualStyleBackColor = True
        '
        'btnLaunch
        '
        Me.btnLaunch.Location = New System.Drawing.Point(126, 345)
        Me.btnLaunch.Name = "btnLaunch"
        Me.btnLaunch.Size = New System.Drawing.Size(75, 23)
        Me.btnLaunch.TabIndex = 20
        Me.btnLaunch.Text = "Launch"
        Me.btnLaunch.UseVisualStyleBackColor = True
        '
        'tcLauncher
        '
        Me.tcLauncher.Controls.Add(Me.tpEnvironments)
        Me.tcLauncher.Controls.Add(Me.tpPaths)
        Me.tcLauncher.Controls.Add(Me.tpAddins)
        Me.tcLauncher.Location = New System.Drawing.Point(3, 3)
        Me.tcLauncher.Name = "tcLauncher"
        Me.tcLauncher.SelectedIndex = 0
        Me.tcLauncher.Size = New System.Drawing.Size(403, 336)
        Me.tcLauncher.TabIndex = 21
        '
        'tpEnvironments
        '
        Me.tpEnvironments.Controls.Add(Me.grpEnvironment)
        Me.tpEnvironments.Controls.Add(Me.grpStartup)
        Me.tpEnvironments.Location = New System.Drawing.Point(4, 22)
        Me.tpEnvironments.Name = "tpEnvironments"
        Me.tpEnvironments.Padding = New System.Windows.Forms.Padding(3)
        Me.tpEnvironments.Size = New System.Drawing.Size(395, 310)
        Me.tpEnvironments.TabIndex = 0
        Me.tpEnvironments.Text = "Environments"
        Me.tpEnvironments.UseVisualStyleBackColor = True
        '
        'tpPaths
        '
        Me.tpPaths.Controls.Add(Me.lblFrameworkVersion)
        Me.tpPaths.Controls.Add(Me.mskFrameworkVersion)
        Me.tpPaths.Controls.Add(Me.txtUserConfig)
        Me.tpPaths.Controls.Add(Me.lblUserConfig)
        Me.tpPaths.Controls.Add(Me.btnUserConfig)
        Me.tpPaths.Controls.Add(Me.txtXmlPath)
        Me.tpPaths.Controls.Add(Me.lblXmlPath)
        Me.tpPaths.Controls.Add(Me.btnXmlPath)
        Me.tpPaths.Controls.Add(Me.txtHelpPath)
        Me.tpPaths.Controls.Add(Me.lblHelpPath)
        Me.tpPaths.Controls.Add(Me.btnHelpFile)
        Me.tpPaths.Controls.Add(Me.txtFramework)
        Me.tpPaths.Controls.Add(Me.lblFramework)
        Me.tpPaths.Controls.Add(Me.btnWorkbooks)
        Me.tpPaths.Controls.Add(Me.btnFrameworkSelect)
        Me.tpPaths.Controls.Add(Me.txtWorkbooks)
        Me.tpPaths.Controls.Add(Me.lblWorkbooks)
        Me.tpPaths.Location = New System.Drawing.Point(4, 22)
        Me.tpPaths.Name = "tpPaths"
        Me.tpPaths.Padding = New System.Windows.Forms.Padding(3)
        Me.tpPaths.Size = New System.Drawing.Size(395, 310)
        Me.tpPaths.TabIndex = 1
        Me.tpPaths.Text = "Paths"
        Me.tpPaths.UseVisualStyleBackColor = True
        '
        'lblFrameworkVersion
        '
        Me.lblFrameworkVersion.AutoSize = True
        Me.lblFrameworkVersion.Location = New System.Drawing.Point(10, 59)
        Me.lblFrameworkVersion.Name = "lblFrameworkVersion"
        Me.lblFrameworkVersion.Size = New System.Drawing.Size(137, 13)
        Me.lblFrameworkVersion.TabIndex = 0
        Me.lblFrameworkVersion.Text = "Framework Version Number"
        '
        'mskFrameworkVersion
        '
        Me.mskFrameworkVersion.Location = New System.Drawing.Point(153, 56)
        Me.mskFrameworkVersion.Mask = "0"
        Me.mskFrameworkVersion.Name = "mskFrameworkVersion"
        Me.mskFrameworkVersion.Size = New System.Drawing.Size(29, 20)
        Me.mskFrameworkVersion.TabIndex = 1
        '
        'txtUserConfig
        '
        Me.txtUserConfig.Location = New System.Drawing.Point(30, 221)
        Me.txtUserConfig.Name = "txtUserConfig"
        Me.txtUserConfig.Size = New System.Drawing.Size(315, 20)
        Me.txtUserConfig.TabIndex = 24
        '
        'lblUserConfig
        '
        Me.lblUserConfig.AutoSize = True
        Me.lblUserConfig.Location = New System.Drawing.Point(10, 205)
        Me.lblUserConfig.Name = "lblUserConfig"
        Me.lblUserConfig.Size = New System.Drawing.Size(113, 13)
        Me.lblUserConfig.TabIndex = 23
        Me.lblUserConfig.Text = "User Configuration File"
        '
        'btnUserConfig
        '
        Me.btnUserConfig.Location = New System.Drawing.Point(351, 221)
        Me.btnUserConfig.Name = "btnUserConfig"
        Me.btnUserConfig.Size = New System.Drawing.Size(32, 23)
        Me.btnUserConfig.TabIndex = 22
        Me.btnUserConfig.Text = "..."
        Me.btnUserConfig.UseVisualStyleBackColor = True
        '
        'txtXmlPath
        '
        Me.txtXmlPath.Location = New System.Drawing.Point(30, 182)
        Me.txtXmlPath.Name = "txtXmlPath"
        Me.txtXmlPath.Size = New System.Drawing.Size(315, 20)
        Me.txtXmlPath.TabIndex = 21
        '
        'lblXmlPath
        '
        Me.lblXmlPath.AutoSize = True
        Me.lblXmlPath.Location = New System.Drawing.Point(10, 166)
        Me.lblXmlPath.Name = "lblXmlPath"
        Me.lblXmlPath.Size = New System.Drawing.Size(96, 13)
        Me.lblXmlPath.TabIndex = 20
        Me.lblXmlPath.Text = "Function Metadata"
        '
        'btnXmlPath
        '
        Me.btnXmlPath.Location = New System.Drawing.Point(351, 182)
        Me.btnXmlPath.Name = "btnXmlPath"
        Me.btnXmlPath.Size = New System.Drawing.Size(32, 23)
        Me.btnXmlPath.TabIndex = 19
        Me.btnXmlPath.Text = "..."
        Me.btnXmlPath.UseVisualStyleBackColor = True
        '
        'txtHelpPath
        '
        Me.txtHelpPath.Location = New System.Drawing.Point(30, 143)
        Me.txtHelpPath.Name = "txtHelpPath"
        Me.txtHelpPath.Size = New System.Drawing.Size(315, 20)
        Me.txtHelpPath.TabIndex = 18
        '
        'lblHelpPath
        '
        Me.lblHelpPath.AutoSize = True
        Me.lblHelpPath.Location = New System.Drawing.Point(10, 127)
        Me.lblHelpPath.Name = "lblHelpPath"
        Me.lblHelpPath.Size = New System.Drawing.Size(61, 13)
        Me.lblHelpPath.TabIndex = 17
        Me.lblHelpPath.Text = "Help Folder"
        '
        'btnHelpFile
        '
        Me.btnHelpFile.Location = New System.Drawing.Point(351, 143)
        Me.btnHelpFile.Name = "btnHelpFile"
        Me.btnHelpFile.Size = New System.Drawing.Size(32, 23)
        Me.btnHelpFile.TabIndex = 16
        Me.btnHelpFile.Text = "..."
        Me.btnHelpFile.UseVisualStyleBackColor = True
        '
        'tpAddins
        '
        Me.tpAddins.Controls.Add(Me.lbAddins)
        Me.tpAddins.Controls.Add(Me.tstAddins)
        Me.tpAddins.Location = New System.Drawing.Point(4, 22)
        Me.tpAddins.Name = "tpAddins"
        Me.tpAddins.Padding = New System.Windows.Forms.Padding(3)
        Me.tpAddins.Size = New System.Drawing.Size(395, 310)
        Me.tpAddins.TabIndex = 2
        Me.tpAddins.Text = "Addins"
        Me.tpAddins.UseVisualStyleBackColor = True
        '
        'lbAddins
        '
        Me.lbAddins.FormattingEnabled = True
        Me.lbAddins.HorizontalScrollbar = True
        Me.lbAddins.Location = New System.Drawing.Point(6, 13)
        Me.lbAddins.Name = "lbAddins"
        Me.lbAddins.Size = New System.Drawing.Size(377, 173)
        Me.lbAddins.TabIndex = 10
        '
        'tstAddins
        '
        Me.tstAddins.BackColor = System.Drawing.SystemColors.Control
        Me.tstAddins.Dock = System.Windows.Forms.DockStyle.None
        Me.tstAddins.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden
        Me.tstAddins.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.btnAddinInsert, Me.btnAddinDelete, Me.btnAddinRename, Me.btnAddinUp, Me.btnAddinDown})
        Me.tstAddins.Location = New System.Drawing.Point(6, 189)
        Me.tstAddins.Name = "tstAddins"
        Me.tstAddins.RenderMode = System.Windows.Forms.ToolStripRenderMode.System
        Me.tstAddins.Size = New System.Drawing.Size(118, 25)
        Me.tstAddins.TabIndex = 8
        Me.tstAddins.Text = "ToolStrip1"
        '
        'btnAddinInsert
        '
        Me.btnAddinInsert.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnAddinInsert.Image = Global.launcher.My.Resources.Resources.create
        Me.btnAddinInsert.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnAddinInsert.Name = "btnAddinInsert"
        Me.btnAddinInsert.Size = New System.Drawing.Size(23, 22)
        Me.btnAddinInsert.Text = "Insert Addin"
        Me.btnAddinInsert.ToolTipText = "Insert Addin"
        '
        'btnAddinDelete
        '
        Me.btnAddinDelete.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnAddinDelete.Image = Global.launcher.My.Resources.Resources.delete
        Me.btnAddinDelete.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnAddinDelete.Name = "btnAddinDelete"
        Me.btnAddinDelete.Size = New System.Drawing.Size(23, 22)
        Me.btnAddinDelete.Text = "Delete Selected Addin"
        '
        'btnAddinRename
        '
        Me.btnAddinRename.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnAddinRename.Image = Global.launcher.My.Resources.Resources.rename
        Me.btnAddinRename.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnAddinRename.Name = "btnAddinRename"
        Me.btnAddinRename.Size = New System.Drawing.Size(23, 22)
        Me.btnAddinRename.Text = "Edit Selected Addin Name"
        '
        'btnAddinUp
        '
        Me.btnAddinUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnAddinUp.Image = Global.launcher.My.Resources.Resources.up
        Me.btnAddinUp.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnAddinUp.Name = "btnAddinUp"
        Me.btnAddinUp.Size = New System.Drawing.Size(23, 22)
        Me.btnAddinUp.Text = "Move Up"
        '
        'btnAddinDown
        '
        Me.btnAddinDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.btnAddinDown.Image = Global.launcher.My.Resources.Resources.down
        Me.btnAddinDown.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.btnAddinDown.Name = "btnAddinDown"
        Me.btnAddinDown.Size = New System.Drawing.Size(23, 22)
        Me.btnAddinDown.Text = "Move Down"
        '
        'FormMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(407, 370)
        Me.Controls.Add(Me.tcLauncher)
        Me.Controls.Add(Me.btnLaunch)
        Me.Controls.Add(Me.btnClose)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Name = "FormMain"
        Me.Text = "QuantLibXL Launcher"
        Me.grpEnvironment.ResumeLayout(False)
        Me.grpEnvironment.PerformLayout()
        Me.tstEnvironment.ResumeLayout(False)
        Me.tstEnvironment.PerformLayout()
        Me.grpStartup.ResumeLayout(False)
        Me.grpStartup.PerformLayout()
        Me.tcLauncher.ResumeLayout(False)
        Me.tpEnvironments.ResumeLayout(False)
        Me.tpPaths.ResumeLayout(False)
        Me.tpPaths.PerformLayout()
        Me.tpAddins.ResumeLayout(False)
        Me.tpAddins.PerformLayout()
        Me.tstAddins.ResumeLayout(False)
        Me.tstAddins.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents btnClose As System.Windows.Forms.Button
    Friend WithEvents lblPreconfigured As System.Windows.Forms.Label
    Friend WithEvents lblFramework As System.Windows.Forms.Label
    Friend WithEvents txtFramework As System.Windows.Forms.TextBox
    Friend WithEvents btnFrameworkSelect As System.Windows.Forms.Button
    Friend WithEvents btnWorkbooks As System.Windows.Forms.Button
    Friend WithEvents txtWorkbooks As System.Windows.Forms.TextBox
    Friend WithEvents lblWorkbooks As System.Windows.Forms.Label
    Friend WithEvents grpEnvironment As System.Windows.Forms.GroupBox
    Friend WithEvents lstPreconfigured As System.Windows.Forms.ListBox
    Friend WithEvents grpStartup As System.Windows.Forms.GroupBox
    Friend WithEvents cbYCBootstrap As System.Windows.Forms.CheckBox
    Friend WithEvents lblUserConfigured As System.Windows.Forms.Label
    Friend WithEvents lstUserconfigured As System.Windows.Forms.ListBox
    Friend WithEvents tstEnvironment As System.Windows.Forms.ToolStrip
    Friend WithEvents btnNew As System.Windows.Forms.ToolStripButton
    Friend WithEvents btnCopy As System.Windows.Forms.ToolStripButton
    Friend WithEvents btnDelete As System.Windows.Forms.ToolStripButton
    Friend WithEvents btnClear As System.Windows.Forms.ToolStripButton
    Friend WithEvents btnRename As System.Windows.Forms.ToolStripButton
    Friend WithEvents btnLaunch As System.Windows.Forms.Button
    Friend WithEvents cbIndexesTimeSeries As System.Windows.Forms.CheckBox
    Friend WithEvents cbCapVolBootstrap As System.Windows.Forms.CheckBox
    Friend WithEvents cbLoadMurexYC As System.Windows.Forms.CheckBox
    Friend WithEvents tcLauncher As System.Windows.Forms.TabControl
    Friend WithEvents tpEnvironments As System.Windows.Forms.TabPage
    Friend WithEvents tpPaths As System.Windows.Forms.TabPage
    Friend WithEvents cbFitCMS As System.Windows.Forms.CheckBox
    Friend WithEvents cbLoadBonds As System.Windows.Forms.CheckBox
    Friend WithEvents cbStaticData As System.Windows.Forms.CheckBox
    Friend WithEvents cbSwapSmileBootstrap As System.Windows.Forms.CheckBox
    Friend WithEvents txtHelpPath As System.Windows.Forms.TextBox
    Friend WithEvents lblHelpPath As System.Windows.Forms.Label
    Friend WithEvents btnHelpFile As System.Windows.Forms.Button
    Friend WithEvents txtUserConfig As System.Windows.Forms.TextBox
    Friend WithEvents lblUserConfig As System.Windows.Forms.Label
    Friend WithEvents btnUserConfig As System.Windows.Forms.Button
    Friend WithEvents txtXmlPath As System.Windows.Forms.TextBox
    Friend WithEvents lblXmlPath As System.Windows.Forms.Label
    Friend WithEvents btnXmlPath As System.Windows.Forms.Button
    Friend WithEvents tpAddins As System.Windows.Forms.TabPage
    Friend WithEvents lblFrameworkVersion As System.Windows.Forms.Label
    Friend WithEvents mskFrameworkVersion As System.Windows.Forms.MaskedTextBox
    Friend WithEvents tstAddins As System.Windows.Forms.ToolStrip
    Friend WithEvents btnAddinInsert As System.Windows.Forms.ToolStripButton
    Friend WithEvents btnAddinDelete As System.Windows.Forms.ToolStripButton
    Friend WithEvents btnAddinUp As System.Windows.Forms.ToolStripButton
    Friend WithEvents btnAddinDown As System.Windows.Forms.ToolStripButton
    Friend WithEvents lbAddins As System.Windows.Forms.ListBox
    Friend WithEvents btnAddinRename As System.Windows.Forms.ToolStripButton
End Class
