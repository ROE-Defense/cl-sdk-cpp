#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "CorticalLabs/CorticalLabs.hpp"

namespace py = pybind11;

PYBIND11_MODULE(cl_sdk_cpp, m) {
    m.doc() = "Python bindings for Cortical Labs C++ SDK";

    py::enum_<cortical_labs::DishConnection::TickRate>(m, "TickRate")
        .value("HZ_30", cortical_labs::DishConnection::TickRate::HZ_30)
        .value("HZ_60", cortical_labs::DishConnection::TickRate::HZ_60)
        .value("HZ_90", cortical_labs::DishConnection::TickRate::HZ_90)
        .value("HZ_120", cortical_labs::DishConnection::TickRate::HZ_120)
        .value("HZ_144", cortical_labs::DishConnection::TickRate::HZ_144)
        .value("UNLOCKED", cortical_labs::DishConnection::TickRate::UNLOCKED)
        .export_values();

    py::class_<cortical_labs::DishConnection>(m, "CorticalLabsClient")
        .def(py::init<const std::string &, const std::string &, bool, cortical_labs::DishConnection::TickRate>(),
             py::arg("endpoint"), py::arg("api_key") = "", py::arg("use_websockets") = true, py::arg("target_hz") = cortical_labs::DishConnection::TickRate::HZ_60)
        .def("connect", &cortical_labs::DishConnection::connect)
        .def("sendSensorData", [](cortical_labs::DishConnection &self, uint32_t timestamp, py::array_t<float> data_x, py::array_t<float> data_y) {
            auto req_x = data_x.request();
            auto req_y = data_y.request();
            std::vector<float> vec_x(static_cast<float*>(req_x.ptr), static_cast<float*>(req_x.ptr) + req_x.size);
            std::vector<float> vec_y(static_cast<float*>(req_y.ptr), static_cast<float*>(req_y.ptr) + req_y.size);
            self.sendSensorData(timestamp, vec_x, vec_y);
        }, py::arg("timestamp"), py::arg("data_x"), py::arg("data_y"))
        .def("receiveSpikes", [](cortical_labs::DishConnection &self, int max_spikes) {
            auto raw_spikes = self.receiveSpikes(max_spikes);
            std::vector<int> spikes_out(59, 0);
            for (const auto& sp : raw_spikes) {
                if (sp.channel_id < 59) {
                    spikes_out[sp.channel_id] = 1;
                }
            }
            return spikes_out;
        }, py::arg("max_spikes") = 59);
}
