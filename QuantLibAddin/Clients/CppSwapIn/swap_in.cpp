
/*
 Copyright (C) 2007 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <Addins/Cpp/addincpp.hpp>
#include <Clients/Cpp/set_fixings.hpp>
#include <ql/qldefines.hpp>
#if defined BOOST_MSVC
#include <oh/auto_link.hpp>
#include <ql/auto_link.hpp>
#endif
#include <iostream>

#define OH_NULL ObjectHandler::Variant()

using namespace QuantLibAddinCpp;

int main() {

    try {

        // Initialize the environment

        initializeAddin();

        ohSetLogFile("qlademo.log", 4L, OH_NULL);
        ohSetConsole(1, 4L, OH_NULL);
        LOG_MESSAGE("Begin example program.");
        LOG_MESSAGE("QuantLibAddin version = " << qlAddinVersion(OH_NULL));
        LOG_MESSAGE("ObjectHandler version = " << ohVersion(OH_NULL));

        // Set the evaluation date to 12 July 2007

        qlSettingsSetEvaluationDate(39275L, OH_NULL);

        // Initialize some values

        std::string yieldCurveID = "EUR_YC";

        // Deserialize the objects

        ohObjectLoad("qlxl_demo_market.xml", OH_NULL, OH_NULL);
        ohObjectLoad("qlxl_demo_swap.xml", OH_NULL, OH_NULL);

        // Enable extrapolation for the yield curve

        qlExtrapolatorEnableExtrapolation(yieldCurveID, true, OH_NULL);
        qlRelinkableHandleLinkTo("EuriborYC3M", yieldCurveID, OH_NULL);
        qlRelinkableHandleLinkTo("EuriborYC6M", yieldCurveID, OH_NULL);

        // Check that the yield curve bootstraps OK

        //std::vector<ObjectHandler::Variant> referenceDates;
        //referenceDates.push_back(qlTermStructureReferenceDate(yieldCurveID));
        //std::vector<double> refDiscounts = qlYieldTSDiscount(yieldCurveID, referenceDates, OH_NULL);
        //LOG_MESSAGE("yts reference discount = " << refDiscounts[0]);

        //std::vector<ObjectHandler::Variant> maxDates;
        //maxDates.push_back(qlTermStructureMaxDate(yieldCurveID));
        //std::vector<double> maxDiscounts = qlYieldTSDiscount(yieldCurveID, maxDates, OH_NULL);
        //LOG_MESSAGE("yts max discount       = " << maxDiscounts[0]);

        // Set the index fixings and verify a couple of them

        //setFixings();

        //std::vector<ObjectHandler::Variant> fixingDates1;
        //fixingDates1.push_back(39220L);     // 18-May-2007
        //std::vector<double> fixing1 = qlIndexFixing("EuriborSwapFIXA5Y", fixingDates1, OH_NULL);
        //LOG_MESSAGE("EuriborSwapFIXA5Y 18-May-2007 " << fixing1[0]);

        //std::vector<ObjectHandler::Variant> fixingDates2;
        //fixingDates1.push_back(38853L);     // 16-May-2006
        //std::vector<double> fixing2 = qlIndexFixing("EURIBOR6M", fixingDates2, OH_NULL);
        //LOG_MESSAGE("EURIBOR6M         16-May-2006 " << fixing2[0]);

        // Set the pricers for the legs of the deal

        //qlLegSetCouponPricer("leg0", "cms_pricer");
        //qlLegSetCouponPricer("leg1", "ibor_pricer");

        // Output the PV of the deal

        LOG_MESSAGE("SWAP PV = " << qlInstrumentNPV("swap", OH_NULL));

        LOG_MESSAGE("End example program.");

        std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?> \
<!DOCTYPE boost_serialization> \
<boost_serialization signature=\"serialization::archive\" version=\"4\"> \
<object_list class_id=\"1\" tracking_level=\"1\" version=\"0\" object_id=\"_0\"> \
	<count>1</count> \
	<item_version>1</item_version> \
	<item class_id=\"0\" tracking_level=\"0\" version=\"1\"> \
		<px class_id=\"135\" tracking_level=\"1\" version=\"0\" object_id=\"_1\"> \
			<ObjectId>test_quote</ObjectId> \
			<ClassName>qlSimpleQuote</ClassName> \
			<Value class_id=\"189\" tracking_level=\"0\" version=\"0\"> \
				<variant_ class_id=\"190\" tracking_level=\"0\" version=\"0\"> \
					<which>1</which> \
					<value>4.5599999999999996</value> \
				</variant_> \
			</Value> \
			<TickValue>0</TickValue> \
			<Permanent>0</Permanent> \
		</px> \
	</item> \
</object_list> \
</boost_serialization";

        std::vector<std::string> idList = ohObjectLoadString(xml, OH_NULL, OH_NULL);
        ohLogObject(idList[0], OH_NULL);

        std::string xml2 = ohObjectSaveString(idList, OH_NULL, OH_NULL);
        LOG_MESSAGE("XML = " << xml2);

        return 0;

    } catch (const std::exception &e) {
        LOG_ERROR("Error: " << e.what());
        return 1;
    } catch (...) {
        LOG_ERROR("Unknown error");
        return 1;
    }

}

