
'Copyright (C) 2007 Eric Ehlers

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

Namespace QuantLibXL

    Public Class StartupActions
        Implements ICloneable
        Implements ISerializable

        ''''''''''''''''''''''''''''''''''''''''''
        ' members
        ''''''''''''''''''''''''''''''''''''''''''

        Private name_ As String = "StartupActions"

        Private ycBootstrap_ As Boolean
        Private loadMurexYC_ As Boolean
        Private capVolBootstrap_ As Boolean
        Private swapVolBootstrap_ As Boolean
        Private swapSmileBootstrap_ As Boolean
        Private fitCMS_ As Boolean
        Private indexesTimeSeries_ As Boolean
        Private loadBonds_ As Boolean
        Private staticData_ As Boolean

        Public Sub serialize(ByRef serializer As ISerializer) Implements ISerializable.serialize

            serializer.serializeProperty(ycBootstrap_, "YieldCurveBootstrap")
            serializer.serializeProperty(loadMurexYC_, "LoadMurexYieldCurve")
            serializer.serializeProperty(capVolBootstrap_, "CapVolBootstrap")
            serializer.serializeProperty(swapVolBootstrap_, "SwapVolBootstrap")
            serializer.serializeProperty(swapSmileBootstrap_, "SwapSmileBootstrap")
            serializer.serializeProperty(fitCMS_, "FitCMS")
            serializer.serializeProperty(indexesTimeSeries_, "IndexesTimeSeries")
            serializer.serializeProperty(loadBonds_, "LoadBonds")
            serializer.serializeProperty(staticData_, "StaticData")

        End Sub

        ''''''''''''''''''''''''''''''''''''''''''
        ' properties
        ''''''''''''''''''''''''''''''''''''''''''

        Public Property Name() As String Implements ISerializable.Name

            Get
                Name = name_
            End Get

            Set(ByVal value As String)
                name_ = value
            End Set

        End Property

        ''''''''''''''''''''''''''''''''''''''''''
        ' properties
        ''''''''''''''''''''''''''''''''''''''''''

        Public Property YieldCurveBootstrap() As Boolean

            Get
                YieldCurveBootstrap = ycBootstrap_
            End Get

            Set(ByVal value As Boolean)
                ycBootstrap_ = value
            End Set

        End Property

        Public Property LoadMurexYieldCurve() As Boolean

            Get
                LoadMurexYieldCurve = loadMurexYC_
            End Get

            Set(ByVal value As Boolean)
                loadMurexYC_ = value
            End Set

        End Property

        Public Property CapVolBootstrap() As Boolean

            Get
                CapVolBootstrap = capVolBootstrap_
            End Get

            Set(ByVal value As Boolean)
                capVolBootstrap_ = value
            End Set

        End Property

        Public Property SwapVolBootstrap() As Boolean

            Get
                SwapVolBootstrap = swapVolBootstrap_
            End Get

            Set(ByVal value As Boolean)
                swapVolBootstrap_ = value
            End Set

        End Property

        Public Property SwapSmileBootstrap() As Boolean

            Get
                SwapSmileBootstrap = swapSmileBootstrap_
            End Get

            Set(ByVal value As Boolean)
                swapSmileBootstrap_ = value
            End Set

        End Property

        Public Property FitCMS() As Boolean

            Get
                FitCMS = fitCMS_
            End Get

            Set(ByVal value As Boolean)
                fitCMS_ = value
            End Set

        End Property

        Public Property IndexesTimeSeries() As Boolean

            Get
                IndexesTimeSeries = indexesTimeSeries_
            End Get

            Set(ByVal value As Boolean)
                indexesTimeSeries_ = value
            End Set

        End Property

        Public Property LoadBonds() As Boolean

            Get
                LoadBonds = loadBonds_
            End Get

            Set(ByVal value As Boolean)
                loadBonds_ = value
            End Set

        End Property

        Public Property StaticData() As Boolean

            Get
                StaticData = staticData_
            End Get

            Set(ByVal value As Boolean)
                staticData_ = value
            End Set

        End Property

        Public Function Clone() As Object Implements ICloneable.Clone

            Clone = Me.MemberwiseClone()

        End Function

    End Class

End Namespace

