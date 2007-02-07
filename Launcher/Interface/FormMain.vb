
'Copyright (C) 2006, 2007 Eric Ehlers

'This file is part of QuantLib, a free-software/open-source library
'for financial quantitative analysts and developers - http://quantlib.org/

'QuantLib is free software: you can redistribute it and/or modify it
'under the terms of the QuantLib license.  You should have received a
'copy of the license along with this program; if not, please email
'<quantlib-dev@lists.sf.net>. The license is also available online at
'<http://quantlib.org/reference/license.html>.

'This program is distributed in the hope that it will be useful, but WITHOUT
'ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
'FOR A PARTICULAR PURPOSE.  See the license for more details.

Imports System.IO
Imports System.Deployment.Application

Public Class FormMain

    ''''''''''''''''''''''''''''''''''''''''''
    ' Private members
    ''''''''''''''''''''''''''''''''''''''''''

    Private config_ As QuantLibXL.Configuration
    Private envUserconfigured_ As QuantLibXL.EnvironmentList
    Private envPreconfigured_ As QuantLibXL.EnvironmentList
    Private selectedEnvironment_ As QuantLibXL.Environment
    Private processingEvents_ As Boolean = True
    Private qlxlDir_ As String

    Private Const PRECONFIGURED As String = "Preconfigured"
    Private Const USERCONFIGURED As String = "UserConfigured"

    ''''''''''''''''''''''''''''''''''''''''''
    ' Properties
    ''''''''''''''''''''''''''''''''''''''''''

    Private Property SelectedEnvironment() As QuantLibXL.Environment

        Get
            If selectedEnvironment_ Is Nothing Then
                Throw New Exception("No environment selected.")
            Else
                SelectedEnvironment = selectedEnvironment_
            End If
        End Get

        Set(ByVal value As QuantLibXL.Environment)
            If value Is Nothing Then
                Throw New Exception("No environment selected.")
            Else
                selectedEnvironment_ = value
                resetControls()
                Me.Text = "QuantLibXL Launcher - " & selectedEnvironment_.Name
            End If
        End Set

    End Property

    ''''''''''''''''''''''''''''''''''''''''''
    ' Initialization
    ''''''''''''''''''''''''''''''''''''''''''

    Public Sub New()

        qlxlDir_ = Environ(QUANTLIBXL_DIR)  ' not fatal if invalid

        InitializeComponent()

        Try

            Upgrade.run()
            initializeConfig()
            overrideActions()
            initializeLists()
            selectEnvironment()

        Catch ex As Exception

            MsgBox(ex.Message, MsgBoxStyle.OkOnly + MsgBoxStyle.Critical, _
                "QuantLibXL Fatal Error")
            Environment.Exit(1)

        End Try

    End Sub

    Private Sub initializeLists()

        For Each environment As QuantLibXL.Environment In envPreconfigured_.Environments
            lstPreconfigured.Items.Add(environment.Name)
        Next

        For Each environment As QuantLibXL.Environment In envUserconfigured_.Environments
            lstUserconfigured.Items.Add(environment.Name)
        Next

    End Sub

    Private Sub selectEnvironment()

        ' Attempt to select the environment that was active
        ' last time the app shut down.  The index that was saved before
        ' may be invalid if the configuration has been changed.

        Try

            If config_.SelectedEnvConfig = PRECONFIGURED Then
                If lstPreconfigured.Items.Contains(config_.SelectedEnvName) Then
                    lstPreconfigured.SelectedIndex = lstPreconfigured.Items.IndexOf(config_.SelectedEnvName)
                    Exit Sub
                End If
            ElseIf config_.SelectedEnvConfig = USERCONFIGURED Then
                If lstUserconfigured.Items.Contains(config_.SelectedEnvName) Then
                    lstUserconfigured.SelectedIndex = lstUserconfigured.Items.IndexOf(config_.SelectedEnvName)
                    Exit Sub
                End If
            End If

            ' If none of the above worked then try for a safe option

            If lstPreconfigured.Items.Count >= 1 Then
                lstPreconfigured.SelectedIndex = 0
            Else
                Throw New Exception("Could not identify an environment to activate")
            End If

        Catch ex As Exception

            Throw New Exception("Error activating environment " _
                & ex.Message)

        End Try

    End Sub

    Private Function deriveConfigPath() As String

        deriveConfigPath = configPath() & "\Environments\config.xml"

        If Not fileExists(deriveConfigPath) Then
            Throw New Exception("Error: this application loads configuration information " & _
            "from the following location:" & vbCrLf & vbCrLf & deriveConfigPath & vbCrLf & vbCrLf & _
            "The path appears to be invalid.")
        End If

    End Function

    Private Sub initializeConfig()

        If Not fileExists(EXCEL_PATH) Then
            Throw New Exception("Error: this application is configured to load Excel " & _
            "from the following location:" & vbCrLf & vbCrLf & EXCEL_PATH & vbCrLf & vbCrLf & _
            "the path appears to be invalid")
        End If

        ' Derive the location of the launcher configuration file config.xml:
        ' - If this application is being run across the network (a ClickOnce installation)
        '   then get the config file from the local ClickOnce directory
        ' - If this application is being run from the command line then
        '   look for config.xml in the location specified by QUANTLIBXL_CONFIG_PATH

        Dim configPath As String
        If (ApplicationDeployment.IsNetworkDeployed) Then
            configPath = ApplicationDeployment.CurrentDeployment.DataDirectory & "\Environments\config.xml"
        Else
            configPath = deriveConfigPath()
        End If

        Try
            Dim xmlReader As New QuantLibXL.XmlReader(configPath, "Configuration")
            xmlReader.serializeObject(envPreconfigured_, "Environments", THIS_VERSION)
        Catch ex As Exception
            Throw New Exception("Error processing configuration file " & configPath & ":" _
                & vbCrLf & vbCrLf & ex.Message)
        End Try

        ' Call the setDotNetParameters() method of the Environment objects to give
        ' them a chance to initialize properties derived from the .NET environment

        envPreconfigured_.setDotNetParameters()

        envPreconfigured_.validate()

        ' Load the user's preferences from the Windows registry.  The config_
        ' object allows the user to override certain characteristics of the
        ' preconfigured environments loaded above, and the envUserconfigured_
        ' object contains additional environments configured by the user.

        Try
            Dim registryReader As New QuantLibXL.RegistryReader()
            registryReader.serializeObject(config_, "Configuration", THIS_VERSION)
            registryReader.serializeObject(envUserconfigured_, "Environments", THIS_VERSION)
        Catch ex As Exception
            Throw New Exception("Error accessing the Windows registry:" _
                 & vbCrLf & vbCrLf & ex.Message)
        End Try

    End Sub

    Private Sub overrideActions()

        ' Preconfigured environments have been loaded from the configuration file.
        ' For each of these, override the startup actions with values saved to
        ' the user's registry from previous session (if any)

        Try

            Dim envPreconfigured As QuantLibXL.Environment
            Dim startupActions As QuantLibXL.StartupActions
            For Each startupActions In config_.OverrideActions.StartupActionsList
                If envPreconfigured_.nameInUse(startupActions.Name) Then
                    envPreconfigured = envPreconfigured_.nameToEnvironment(startupActions.Name)
                    envPreconfigured.StartupActions = CType(startupActions, QuantLibXL.StartupActions).Clone
                End If
            Next

        Catch ex As Exception
            Throw New Exception("Error deriving startup actions: " & ex.Message)
        End Try

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Events - main form
    ''''''''''''''''''''''''''''''''''''''''''

    Private Sub btnClose_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClose.Click

        Try
            config_.OverrideActions.clear()
            For Each environment As QuantLibXL.Environment In envPreconfigured_.Environments
                config_.OverrideActions.add(environment.StartupActions, environment.Name)
            Next

            Dim registryWriter As New QuantLibXL.RegistryWriter()
            registryWriter.deleteKey("Configuration")
            registryWriter.serializeObject(config_, "Configuration", THIS_VERSION)
            registryWriter.deleteKey("Environments")
            registryWriter.serializeObject(envUserconfigured_, "Environments", THIS_VERSION)
        Catch ex As Exception
            MsgBox("Error while closing launcher:" & vbCrLf & vbCrLf & ex.Message, _
                MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, "QuantLibXL Error")
        End Try

        Close()

    End Sub

    Private Sub btnLaunch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnLaunch.Click

        Try

            SelectedEnvironment.launch()

        Catch ex As Exception

            MsgBox("Error on launch command:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Events - environment - lists
    ''''''''''''''''''''''''''''''''''''''''''

    Private Sub lstPreconfigured_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lstPreconfigured.SelectedIndexChanged

        Try

            If lstPreconfigured.SelectedIndex = -1 Then Exit Sub
            If processingEvents_ Then
                processingEvents_ = False
                SelectedEnvironment = envPreconfigured_.nameToEnvironment(lstPreconfigured.Text)
                lstUserconfigured.SelectedIndex = -1
                setEnabled(False)
                processingEvents_ = True
                config_.SelectedEnvConfig = PRECONFIGURED
                config_.SelectedEnvName = lstPreconfigured.Text
            End If

        Catch ex As Exception

            MsgBox("Error processing environment selection:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub lstUserconfigured_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lstUserconfigured.SelectedIndexChanged

        Try

            If lstUserconfigured.SelectedIndex = -1 Then Exit Sub
            If processingEvents_ Then
                processingEvents_ = False
                SelectedEnvironment = envUserconfigured_.nameToEnvironment(lstUserconfigured.Text)
                lstPreconfigured.SelectedIndex = -1
                setEnabled(True)
                processingEvents_ = True
                config_.SelectedEnvConfig = USERCONFIGURED
                config_.SelectedEnvName = lstUserconfigured.Text
            End If

        Catch ex As Exception

            MsgBox("Error processing environment selection:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Events - environment - buttons
    ''''''''''''''''''''''''''''''''''''''''''

    Private Function inputEnvironmentName(ByVal initialValue As String) As String

        inputEnvironmentName = initialValue
        Dim invalidEntry As Boolean = True
        While invalidEntry
            inputEnvironmentName = InputBox("Edit Environment Name:", _
                "Environment Name", inputEnvironmentName)
            If Len(inputEnvironmentName) < 1 Then Exit Function
            invalidEntry = envUserconfigured_.nameInUse(inputEnvironmentName)
            If invalidEntry Then
                MsgBox("The name '" & inputEnvironmentName & "' is already in use - " _
                & "please enter a different name.")
            End If
        End While

    End Function

    Private Sub btnNew_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnNew.Click

        Try

            Dim newName As String = inputEnvironmentName(envUserconfigured_.deriveNewName())
            If Len(newName) < 1 Then Exit Sub

            SelectedEnvironment = envUserconfigured_.createEnvironment(newName)
            Dim newIndex As Integer = lstUserconfigured.Items.Add(newName)
            lstUserconfigured.SelectedIndex = newIndex

        Catch ex As Exception

            MsgBox("Error creating new environment:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub btnCopy_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCopy.Click

        Try

            Dim newName As String = inputEnvironmentName(envUserconfigured_.deriveCopyName(SelectedEnvironment.Name))
            If Len(newName) < 1 Then Exit Sub

            SelectedEnvironment = envUserconfigured_.copyEnvironment(SelectedEnvironment, newName)
            Dim newIndex As Integer = lstUserconfigured.Items.Add(newName)
            lstUserconfigured.SelectedIndex = newIndex

        Catch ex As Exception

            MsgBox("Error copying environment:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub btnDelete_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDelete.Click

        Try

            If lstUserconfigured.SelectedIndex = -1 Then
                MsgBox("No environment selected", _
                    MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                    "QuantLibXL Error")
                Exit Sub
            End If

            envUserconfigured_.deleteEnvironment(lstUserconfigured.Text)
            Dim deletedIndex As Integer = lstUserconfigured.SelectedIndex
            lstUserconfigured.Items.RemoveAt(lstUserconfigured.SelectedIndex)

            If lstUserconfigured.Items.Count > 0 Then
                lstUserconfigured.SelectedIndex = Math.Min(deletedIndex, lstUserconfigured.Items.Count - 1)
            Else
                lstPreconfigured.SelectedIndex = 0
            End If

        Catch ex As Exception

            MsgBox("Error deleting environment:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub btnClear_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClear.Click

        Try

            clear()

        Catch ex As Exception

            MsgBox("Error clearing environment:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub btnRename_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRename.Click

        If lstUserconfigured.SelectedIndex = -1 Then Exit Sub

        Try

            Dim oldName As String = SelectedEnvironment.Name
            Dim newName As String = inputEnvironmentName(oldName)
            If Len(newName) < 1 Or newName = oldName Then Exit Sub

            envUserconfigured_.renameEnvironment(oldName, newName)
            SelectedEnvironment.Name = newName
            config_.SelectedEnvName = newName
            processingEvents_ = False
            lstUserconfigured.Items.Remove(oldName)
            Dim newIndex As Integer = lstUserconfigured.Items.Add(newName)
            lstUserconfigured.SelectedIndex = newIndex
            processingEvents_ = True

        Catch ex As Exception

            MsgBox("Error renaming environment:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Events - startup actions
    ''''''''''''''''''''''''''''''''''''''''''

    Private Sub cbYCBootstrap_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbYCBootstrap.CheckedChanged
        SelectedEnvironment.StartupActions.YieldCurveBootstrap = cbYCBootstrap.Checked
    End Sub

    Private Sub cbLoadMurexYC_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbLoadMurexYC.CheckedChanged
        SelectedEnvironment.StartupActions.LoadMurexYieldCurve = cbLoadMurexYC.Checked
    End Sub

    Private Sub cbCapVolBootstrap_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbCapVolBootstrap.CheckedChanged
        SelectedEnvironment.StartupActions.CapVolBootstrap = cbCapVolBootstrap.Checked
    End Sub

    Private Sub cbSwapSmileBootstrap_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbSwapSmileBootstrap.CheckedChanged
        SelectedEnvironment.StartupActions.SwapSmileBootstrap = cbSwapSmileBootstrap.Checked
    End Sub

    Private Sub cbFitCMS_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbFitCMS.CheckedChanged
        SelectedEnvironment.StartupActions.FitCMS = cbFitCMS.Checked
    End Sub

    Private Sub cbIndexesTimeSeries_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbIndexesTimeSeries.CheckedChanged
        SelectedEnvironment.StartupActions.IndexesTimeSeries = cbIndexesTimeSeries.Checked
    End Sub

    Private Sub cbLoadBonds_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbLoadBonds.CheckedChanged
        SelectedEnvironment.StartupActions.LoadBonds = cbLoadBonds.Checked
    End Sub

    Private Sub cbStaticData_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cbStaticData.CheckedChanged
        SelectedEnvironment.StartupActions.StaticData = cbStaticData.Checked
    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Events - path - buttons
    ''''''''''''''''''''''''''''''''''''''''''

    Private Function deriveDefaultDir(ByVal testPath As String, ByVal subDir As String) As String

        Try

            If dirExists(testPath) Then
                deriveDefaultDir = testPath
            ElseIf dirExists(qlxlDir_ & "\" & subDir) Then
                deriveDefaultDir = qlxlDir_ & "\" & subDir
            Else
                deriveDefaultDir = ""
            End If
            Exit Function

        Catch ex As Exception

            deriveDefaultDir = ""

        End Try

    End Function

    Private Function deriveDefaultFile(ByVal testFile As String, ByVal relativePath As String) As String

        Try

            If fileExists(testFile) Then
                deriveDefaultFile = testFile
            ElseIf dirExists(qlxlDir_ & "\" & relativePath) Then
                deriveDefaultFile = qlxlDir_ & "\" & relativePath
            Else
                deriveDefaultFile = ""
            End If
            Exit Function

        Catch ex As Exception

            deriveDefaultFile = ""

        End Try

    End Function

    Private Sub btnFrameworkSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFrameworkSelect.Click

        Try

            Dim dlg As New OpenFileDialog()
            dlg.InitialDirectory = deriveDefaultFile(txtFramework.Text, "framework")
            dlg.FileName = "QuantLibXL.xla"
            dlg.Filter = "Excel VBA Addins (*.xla)|*.xla"
            dlg.Title = "Select QuantLibXL VBA Framework"
            If dlg.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                txtFramework.Text = dlg.FileName
            End If

        Catch ex As Exception

            MsgBox("Error processing framework path:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub btnWorkbooks_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWorkbooks.Click

        Try

            Dim dlg As New FolderBrowserDialog()
            dlg.SelectedPath = deriveDefaultDir(txtWorkbooks.Text, "Workbooks")
            dlg.Description = "Select QuantLibXL Workbook root folder"
            dlg.ShowNewFolderButton = False
            If dlg.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                txtWorkbooks.Text = dlg.SelectedPath
            End If

        Catch ex As Exception

            MsgBox("Error processing workbooks path:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub btnHelpFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHelpFile.Click

        Try

            Dim dlg As New FolderBrowserDialog()
            dlg.SelectedPath = deriveDefaultDir(txtHelpPath.Text, "Docs")
            dlg.Description = "Select QuantLibXL Help folder"
            dlg.ShowNewFolderButton = False
            If dlg.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                txtHelpPath.Text = dlg.SelectedPath
            End If

        Catch ex As Exception

            MsgBox("Error processing helpfile path:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub btnXmlPath_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnXmlPath.Click

        Try

            Dim dlg As New FolderBrowserDialog()
            dlg.SelectedPath = deriveDefaultDir(txtXmlPath.Text, "..\QuantLibAddin\gensrc\metadata")
            dlg.Description = "Select QuantLibXL Function Metadata folder"
            dlg.ShowNewFolderButton = False
            If dlg.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                txtXmlPath.Text = dlg.SelectedPath
            End If

        Catch ex As Exception

            MsgBox("Error processing Function Metadata folder:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    Private Sub btnUserConfig_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUserConfig.Click

        Try

            Dim dlg As New OpenFileDialog()
            dlg.InitialDirectory = deriveDefaultFile(txtUserConfig.Text, "")
            If Len(Path.GetFileName(txtUserConfig.Text)) > 0 Then
                dlg.FileName = Path.GetFileName(txtUserConfig.Text)
            Else
                dlg.FileName = "users.xml"
            End If
            dlg.Filter = "XML Configuration Files (*.xml)|*.xml"
            dlg.Title = "Select QuantLibXL User Configuration File"
            If dlg.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
                txtUserConfig.Text = dlg.FileName
            End If

        Catch ex As Exception

            MsgBox("Error processing User Configuration File path:" _
                 & vbCrLf & vbCrLf & ex.Message, _
                 MsgBoxStyle.OkOnly + MsgBoxStyle.Exclamation, _
                "QuantLibXL Error")

        End Try

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Events - path - text
    ''''''''''''''''''''''''''''''''''''''''''

    Private Sub txtFramework_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtFramework.TextChanged
        SelectedEnvironment.FrameworkName = txtFramework.Text
    End Sub

    Private Sub txtWorkbooks_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtWorkbooks.TextChanged
        SelectedEnvironment.Workbooks = txtWorkbooks.Text
    End Sub

    Private Sub txtHelpFile_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtHelpPath.TextChanged
        SelectedEnvironment.HelpPath = txtHelpPath.Text
    End Sub

    Private Sub txtXmlPath_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtXmlPath.TextChanged
        SelectedEnvironment.XmlPath = txtXmlPath.Text
    End Sub

    Private Sub txtUserConfig_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles txtUserConfig.TextChanged
        SelectedEnvironment.UserConfig = txtUserConfig.Text
    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Events - environment properties
    ''''''''''''''''''''''''''''''''''''''''''

    Private Sub mskFrameworkVersion_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles mskFrameworkVersion.TextChanged

        If Len(mskFrameworkVersion.Text) > 0 Then
            SelectedEnvironment.FrameworkVersion = CInt(mskFrameworkVersion.Text)
        Else
            SelectedEnvironment.FrameworkVersion = 0
        End If

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Events - addins
    ''''''''''''''''''''''''''''''''''''''''''

    ' Sub enableAddinButtons() - Based on the current selection,
    ' enable or disable the following buttons: Delete, Rename, Up, Down

    Private Sub enableAddinButtons()

        If lbAddins.SelectedIndex = -1 Then
            ' Nothing selected - disable all buttons
            btnAddinDelete.Enabled = False
            btnAddinRename.Enabled = False
            btnAddinUp.Enabled = False
            btnAddinDown.Enabled = False
            Exit Sub
        End If

        ' Something is selected, so enable Delete/Rename
        btnAddinDelete.Enabled = True
        btnAddinRename.Enabled = True

        If lbAddins.Items.Count <= 1 Then
            ' Only one item in list - disable Up/Down
            btnAddinUp.Enabled = False
            btnAddinDown.Enabled = False
            Exit Sub
        End If

        If lbAddins.SelectedIndex = 0 Then
            ' Top item selected - disable Up, enable Down
            btnAddinUp.Enabled = False
            btnAddinDown.Enabled = True
        ElseIf lbAddins.SelectedIndex = (lbAddins.Items.Count - 1) Then
            ' Bottom item selected - enable Up, disable Down
            btnAddinUp.Enabled = True
            btnAddinDown.Enabled = False
        Else
            ' Middle item selected - enable Up & Down
            btnAddinUp.Enabled = True
            btnAddinDown.Enabled = True
        End If

    End Sub

    Private Sub synchAddinList()

        Dim addinList(lbAddins.Items.Count - 1) As String
        Dim i As Integer = 0
        For Each item As String In lbAddins.Items
            addinList(i) = item
            i = i + 1
        Next
        SelectedEnvironment.AddinList = addinList

    End Sub

    Private Sub lbAddins_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lbAddins.SelectedIndexChanged

        Call enableAddinButtons()
        Call synchAddinList()

    End Sub

    Private Sub btnAddinInsert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddinInsert.Click

        If lbAddins.Items.Count >= QuantLibXL.Environment.MAX_ADDIN_COUNT Then
            Throw New Exception("You cannot insert another addin into the list" _
             & " because the list currently contains " & lbAddins.Items.Count _
             & " items and the maximum supported by the launcher is " _
             & QuantLibXL.Environment.MAX_ADDIN_COUNT & ".")
        End If

        Dim dlg As New OpenFileDialog()
        dlg.InitialDirectory = deriveDefaultFile(lbAddins.SelectedValue, "addin")
        dlg.FileName = ""
        dlg.Filter = "Excel XLL Addins (*.xll)|*.xll"
        dlg.Title = "Select Addin"

        If dlg.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
            If lbAddins.SelectedIndex = -1 Then
                lbAddins.Items.Add(dlg.FileName)
            Else
                lbAddins.Items.Insert(lbAddins.SelectedIndex, dlg.FileName)
            End If
            Call enableAddinButtons()
            Call synchAddinList()
        End If

    End Sub

    Private Sub btnAddinRename_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddinRename.Click

        Dim newName As String = InputBox("Edit Addin name:", _
            "Edit Addin Name", lbAddins.SelectedItem)
        If Len(newName) > 0 Then
            lbAddins.Items(lbAddins.SelectedIndex) = newName
        End If

    End Sub

    Private Sub btnAddinDelete_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddinDelete.Click

        If lbAddins.SelectedIndex = -1 Then Exit Sub
        Dim i As Integer = lbAddins.SelectedIndex
        lbAddins.Items.RemoveAt(i)
        lbAddins.SelectedIndex = Math.Min(i, lbAddins.Items.Count - 1)
        Call enableAddinButtons()
        Call synchAddinList()

    End Sub

    Private Sub btnAddinUp_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddinUp.Click

        If lbAddins.Items.Count <= 1 Then Exit Sub
        If lbAddins.SelectedIndex < 1 Then Exit Sub
        Dim o As Object = lbAddins.SelectedItem
        Dim i As Integer = lbAddins.SelectedIndex
        lbAddins.Items.RemoveAt(i)
        lbAddins.Items.Insert(i - 1, o)
        lbAddins.SelectedIndex = i - 1

    End Sub

    Private Sub btnAddinDown_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddinDown.Click

        If lbAddins.SelectedIndex = -1 Then Exit Sub
        If lbAddins.Items.Count <= 1 Then Exit Sub
        If lbAddins.SelectedIndex = (lbAddins.Items.Count - 1) Then Exit Sub
        Dim o As Object = lbAddins.SelectedItem
        Dim i As Integer = lbAddins.SelectedIndex
        lbAddins.Items.RemoveAt(i)
        lbAddins.Items.Insert(i + 1, o)
        lbAddins.SelectedIndex = i + 1

    End Sub

    ''''''''''''''''''''''''''''''''''''''''''
    ' Private functions
    ''''''''''''''''''''''''''''''''''''''''''

    Private Sub setEnabled(ByVal enabled As Boolean)

        ' Environments
        btnDelete.Enabled = enabled
        btnClear.Enabled = enabled
        btnRename.Enabled = enabled

        ' Paths - text boxes
        btnFrameworkSelect.Enabled = enabled
        btnWorkbooks.Enabled = enabled
        btnHelpFile.Enabled = enabled
        btnXmlPath.Enabled = enabled
        btnUserConfig.Enabled = enabled

        ' Paths - buttons
        txtFramework.Enabled = enabled
        txtWorkbooks.Enabled = enabled
        txtHelpPath.Enabled = enabled
        txtXmlPath.Enabled = enabled
        txtUserConfig.Enabled = enabled

        ' Environment properties
        mskFrameworkVersion.Enabled = enabled

        ' Addins
        lbAddins.Enabled = enabled
        btnAddinInsert.Enabled = enabled
        If enabled Then
            Call enableAddinButtons()
        Else
            btnAddinUp.Enabled = False
            btnAddinDown.Enabled = False
            btnAddinDelete.Enabled = False
            btnAddinRename.Enabled = False
        End If

    End Sub

    Private Sub clear()

        ' Paths - text boxes
        txtFramework.Clear()
        txtWorkbooks.Clear()
        txtHelpPath.Clear()
        txtXmlPath.Clear()
        txtUserConfig.Clear()

        ' Startup actions
        cbYCBootstrap.Checked = False
        cbLoadMurexYC.Checked = False
        cbCapVolBootstrap.Checked = False
        cbSwapSmileBootstrap.Checked = False
        cbFitCMS.Checked = False
        cbIndexesTimeSeries.Checked = False
        cbLoadBonds.Checked = False
        cbStaticData.Checked = False

        ' Environment properties
        mskFrameworkVersion.Clear()

        ' Addins

    End Sub

    ' After changing the selected environment, call this sub
    ' to synch up the controls.

    Private Sub resetControls()

        ' Paths - text boxes
        txtFramework.Text = SelectedEnvironment.FrameworkName
        txtWorkbooks.Text = SelectedEnvironment.Workbooks
        txtHelpPath.Text = SelectedEnvironment.HelpPath
        txtXmlPath.Text = SelectedEnvironment.XmlPath
        txtUserConfig.Text = SelectedEnvironment.UserConfig

        ' Startup actions
        cbYCBootstrap.Checked = SelectedEnvironment.StartupActions.YieldCurveBootstrap
        cbLoadMurexYC.Checked = SelectedEnvironment.StartupActions.LoadMurexYieldCurve
        cbCapVolBootstrap.Checked = SelectedEnvironment.StartupActions.CapVolBootstrap
        cbSwapSmileBootstrap.Checked = SelectedEnvironment.StartupActions.SwapSmileBootstrap
        cbFitCMS.Checked = SelectedEnvironment.StartupActions.FitCMS
        cbIndexesTimeSeries.Checked = SelectedEnvironment.StartupActions.IndexesTimeSeries
        cbLoadBonds.Checked = SelectedEnvironment.StartupActions.LoadBonds
        cbStaticData.Checked = SelectedEnvironment.StartupActions.StaticData

        ' Environment properties
        mskFrameworkVersion.Text = CStr(SelectedEnvironment.FrameworkVersion)

        ' Addins
        lbAddins.Items.Clear()
        If SelectedEnvironment.AddinList.Length > 0 Then
            For i As Integer = 0 To UBound(SelectedEnvironment.AddinList)
                lbAddins.Items.Add(SelectedEnvironment.AddinList(i))
            Next
        End If

        Call enableAddinButtons()

    End Sub

End Class
