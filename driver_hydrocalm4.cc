/*
 Copyright (C) 2024 Fredrik Öhrström (gpl-3.0-or-later)
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
 
#include"meters_common_implementation.h"
 
namespace
{
    struct Driver : public virtual MeterCommonImplementation
    {
        Driver(MeterInfo &mi, DriverInfo &di);
    };
 
    static bool ok = registerDriver([](DriverInfo&di)
    {
        di.setName("hydrocalm4");
        di.setDefaultFields("name,id,status,total_heating_kwh,total_cooling_kwh,timestamp");
        di.setMeterType(MeterType::HeatMeter);
        di.addLinkMode(LinkMode::T1);
        di.addDetection(MANUFACTURER_BMT, 0x0d,  0x1a);
        di.setConstructor([](MeterInfo& mi, DriverInfo& di){ return std::shared_ptr<Meter>(new Driver(mi, di)); });
    });
 
    Driver::Driver(MeterInfo &mi, DriverInfo &di) :  MeterCommonImplementation(mi, di)
    {
        setMfctTPLStatusBits(
            Translate::Lookup()
            .add(Translate::Rule("TPL_STS", Translate::MapType::BitToString)
                 .set(MaskBits(0xffff))
                 .set(DefaultMessage("OK"))
                 .add(Translate::Map(0x80 ,"SABOTAGE", TestBit::Set))));
 
        addStringField(
            "status",
            "Meter status from tpl status field.",
            DEFAULT_PRINT_PROPERTIES  |
            PrintProperty::STATUS | PrintProperty::INCLUDE_TPL_STATUS);
 
        addNumericFieldWithExtractor(
            "total_heating",
            "The total heating energy consumption recorded by this meter.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            .set(IndexNr(1))
            );
 
        addNumericFieldWithExtractor(
            "device",
            "The date time when the recording was made.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::PointInTime,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::DateTime)
            );
 
        addNumericFieldWithExtractor(
            "total_cooling",
            "The total cooling energy consumption recorded by this meter.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Energy,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyEnergyVIF)
            .set(IndexNr(2))
            );
 
        addNumericFieldWithExtractor(
            "total_heating",
            "Total heating volume of media.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Volume,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Volume)
            .set(IndexNr(1))
            );
 
        addNumericFieldWithExtractor(
            "total_cooling",
            "Total cooling volume of media.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Volume,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Volume)
            .set(IndexNr(2))
            );
 
        addNumericFieldWithExtractor(
            "c1_volume",
            "Supply c1 volume.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Volume,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Volume)
            .set(IndexNr(3))
            );
 
        addNumericFieldWithExtractor(
            "c2_volume",
            "Supply c2 volume.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Volume,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::Volume)
            .set(IndexNr(4))
            );
 
        addNumericFieldWithExtractor(
            "supply_temperature",
            "The supply t1 pipe temperature.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Temperature,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::FlowTemperature)
            .set(IndexNr(1))
            );
 
        addNumericFieldWithExtractor(
            "return_temperature",
            "The supply t2 pipe temperature.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Temperature,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::ReturnTemperature)
            );
 
        addNumericFieldWithExtractor(
            "volume_flow",
            "The current heat media volume flow.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Flow,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::VolumeFlow)
            );
 
        addNumericFieldWithExtractor(
            "power",
            "The current power consumption.",
            DEFAULT_PRINT_PROPERTIES,
            Quantity::Power,
            VifScaling::Auto, DifSignedness::Signed,
            FieldMatcher::build()
            .set(MeasurementType::Instantaneous)
            .set(VIFRange::AnyPowerVIF)
            );
 
     }
}
 
// Test: testm4 hydrocalm4 05171338 NOKEY
// telegram=|2C44B409381317051A0D8C00497A76000000_046D25AA153A0C03000000000C13000000000F6400000000000000|
// {"_":"telegram","media":"heat/cooling load","meter":"hydrocalm4","status":"OK","name":"testm4","id":"05171338","device_datetime":"2024-10-21 10:37","total_heating_kwh":0}
// |testm4;05171338;OK;0;null;1111-11-11 11:11.11
 
// Test: testm4 hydrocalm4 05171338 NOKEY
// telegram=|3A44B409381317051A0D8C00497A7A000000_046D29AA153A0C03000000000C13000000008C1003050000008C1013040000000F6401000000000000|
// {"_":"telegram","media":"heat/cooling load","meter":"hydrocalm4","name":"testm4","id":"05171338","device_datetime":"2024-10-21 10:41","total_cooling_kwh":0.005}
// |testm4;05171338;OK;0;0.005;1111-11-11 11:11.11
 
// Test: testm4 hydrocalm4 05171338 NOKEY
// telegram=|3E44B409381317051A0D8C00497A7C000000_046D2BAA153A0C03000000000C13000000000B3B0000000B280000000A5930230A5D08250F6402000000000000|
// {"_":"telegram","media":"heat/cooling load","meter":"hydrocalm4","name":"testm4","id":"05171338","device_datetime":"2024-10-21 10:43","power_kw": 0}
// |testm4;05171338;OK;0;null;1111-11-11 11:11.11
 
// Test: testm4 hydrocalm4 05171338 NOKEY
// telegram=|3B44B409381317051A0D8C00497A7E000000_046D2DAA153A0C03000000000C13000000008C4013999900008C804013888800000F6403000000000000|
// {"_":"telegram","c1_volume_m3": 9.999,"c2_volume_m3": 8.888,"device_datetime": "2024-10-21 10:45","id": "05171338","media": "heat/cooling load","meter": "hydrocalm4","name": "testm4"}
// |testm4;05171338;OK;0;null;1111-11-11 11:11.11
 
// Test: testm5 hydrocalm4 05128041 NOKEY
// telegram=|3e44B409418012051a0d8c20f17a9d000020046d0d3126310c0a481878330c13098405000B3B0000000B301000000a5929360a5d94230f6402000000000000|
// {"_": "telegram","device_datetime": "2025-01-06 17:13","id": "05128041","media": "heat/cooling load","meter": "hydrocalm4","name": "testm5","power_kw": 3e-06,"return_temperature_c": 23.94,"status": "OK","supply_temperature_c": 36.29,"total_heating_kwh": 938.384667}
// |testm5;05128041;OK;938.384667;null;1111-11-11 11:11.11