#include <iostream>
#include <memory>
#include <stdexcept>
#include <cmath>

#include "config/Config.hh"
#include "detector/GasModel.hh"
#include "detector/FieldManager.hh"
#include "detector/DriftEngine.hh"
#include "detector/HATGeometry.hh"
#include "electronics/WaveformBuilder.hh"
#include "generator/PrimaryGenerator.hh"
#include "generator/LaserGenerator.hh"
#include "generator/TrackGenerator.hh"
#include "io/EventWriter.hh"
#include "io/ROOTOutput.hh"
#include "io/ND280Output.hh"


int main(int argc, char* argv[]) {
    // ─────────────────────────────────────────────────────────────────────────────
    // 0. Ascii
    // ─────────────────────────────────────────────────────────────────────────────
    std::cout << R"(
                            @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
                           @      @@@@  @@@@    @       @      @    @    @@@@    @@@                                     @ +
                          @      @@@   @      @   @    @    @ @ @   @@@  @@     @  @                                   .@  +
                        :# \/   @      @@@@     @     @@@@ @    @     @  @@@   @  @@@                                 @    +
                       @  - .--@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@       @     +
                      @    /\                                                                               @       @      +
                     @                                                                                     @      .%       +
                   #:                                                                                     @      @         +
                  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          +
                  @     @@@@@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@     @          +
                  @     @                *    -                @   @                 @   @           :@   @     @          +
                  @     @                *    -                @   @                 @   @          @     @     @          +
                  @     @                *    -                @   @                 @   @         @      @     @          +
                  @     @                *    -                @   @                 @   @       @.       @     @          +
                  @     @                *    -                @   @                 @   @      @         @     @          +
                  @     @                *    -                @   @                 @   @    +*          @     @          +
                  @     @@@@@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@     @          +
                  @                                                                         @                   @         ++
                  @     @@@@@@@@@@@@@@@@@*    -@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@     @        @
                  @     @                *    -                @   @                 @   @                @     @       @
                  @     @                *    -                @   @                 @ #:@                @     @     .@
                  @     @                *    -                @   @                 @   @                @     @    @.
                  @     @                *    -                @   @                 @   @                @     @   @
                  @     @                *    -                @   @                 @   @                @     @  @
                  @     @@@@@@@@@@@@@@@@@*    -@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@@   @@@@@@@@@@@@@@@@@@     @:#
                  @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@
    )" << "\n";

    // ------------------------------------------------------------------
    // 1. Load configuration
    // ------------------------------------------------------------------
    const std::string configFile = (argc > 1) ? argv[1] : "config/simulation.json";
    std::cout << "[tpcmc] Loading config: " << configFile << "\n";

    Config cfg;
    try {
        cfg = Config::Load(configFile);
    } catch (const std::exception& e) {
        std::cerr << "[tpcmc] Config error: " << e.what() << "\n";
        return 1;
    }

    const int outputLevel = cfg.run.outputLevel;
    std::cout << "[tpcmc] Output level: " << outputLevel << "\n";

    TRandom3 *rand = new TRandom3(cfg.run.randomSeed);

    // ------------------------------------------------------------------
    // 2. Setup detector
    // ------------------------------------------------------------------
    HATGeometry hat;
    double eram3Ymin = hat.GetERAM(3).centerY - hat.kERAMHeightY/2.;
    double eram3Ymax = hat.GetERAM(3).centerY + hat.kERAMHeightY/2.;
    double eram3Zmin = hat.GetERAM(3).centerZ - hat.kERAMWidthZ/2.;
    double eram3Zmax = hat.GetERAM(3).centerZ + hat.kERAMWidthZ/2.;
    
    GasModel gas;
    gas.Initialize(cfg);

    FieldManager field;
    field.Initialize(cfg, gas);

    DriftEngine drift;
    drift.Initialize(field.GetSensor(), cfg);

    // ------------------------------------------------------------------
    // 3. Setup primary generator
    // ------------------------------------------------------------------
    std::unique_ptr<PrimaryGenerator> generator;
    if (cfg.generator.type == "laser") {
        generator = std::make_unique<LaserGenerator>(cfg, rand);
    } else if (cfg.generator.type == "track") {
        generator = std::make_unique<TrackGenerator>(cfg, field.GetSensor(), rand);
    } else {
        std::cerr << "[tpcmc] Unknown generator type: " << cfg.generator.type << "\n";
        return 1;
    }
    generator->Initialize();

    // ------------------------------------------------------------------
    // 5. Setup output
    // ------------------------------------------------------------------
    std::unique_ptr<EventWriter> writer;
    const auto& fmt = cfg.run.outputFormat;
    if (fmt == "nd280") {
        writer = std::make_unique<ND280Output>();
    } else if (fmt == "root" || fmt.empty()) {
        writer = std::make_unique<ROOTOutput>();
    } else {
        std::cerr << "[tpcmc] Unknown output_format: " << fmt << "\n";
        return 1;
    }
    writer->Initialize(cfg.run.outputFile, outputLevel, cfg);

    std::unique_ptr<WaveformBuilder> waveformBuilder;
    if (outputLevel >= 3) {
        waveformBuilder = std::make_unique<WaveformBuilder>(cfg);
    }

    // ------------------------------------------------------------------
    // 6. Event loop
    // ------------------------------------------------------------------
    std::cout << "[tpcmc] Starting event loop: " << cfg.run.nEvents << " events\n";

    for (int ev = 0; ev < cfg.run.nEvents; ++ev) {
        if (ev % 100 == 0)
            std::cout << "[tpcmc] Event " << ev << " / " << cfg.run.nEvents
                      << "\r" << std::flush;

        auto primaries = generator->Generate();

        // ── Level 1 (always): cluster — primary electrons before drift ─────

        ClusterEventData clData;
        clData.eventId    = ev;
        clData.nElectrons = static_cast<int>(primaries.size());
        for (const auto& pe : primaries) {
            // remove cluster in front of ERAM3 position
            if(pe.y > eram3Ymin && pe.y < eram3Ymax && pe.z > eram3Zmin && pe.z < eram3Zmax) {
                continue;
            }
            clData.x.push_back(static_cast<float>(pe.x));
            clData.y.push_back(static_cast<float>(pe.y));
            clData.z.push_back(static_cast<float>(pe.z));
            clData.t.push_back(static_cast<float>(pe.t));
            clData.ekin.push_back(static_cast<float>(pe.ekin));
        }
        writer->WriteEvent(clData, generator->GetTruePositions(), generator->GetTrueDirections());

        // ── Level 2+: drift once, fill anode ───
        if (outputLevel >= 2) {
            AnodeEventData anData;
            anData.eventId    = ev;
            anData.nPrimaries = static_cast<int>(primaries.size());

            for (const auto& pe : primaries) {
                auto endpoints = drift.DriftElectron(pe.x, pe.y, pe.z, pe.t);
                for (const auto& ep : endpoints) {
                    // anode record (level 2)
                    if(ep.z < cfg.detector.tpcZminMm || ep.z > cfg.detector.tpcZmaxMm ||
                       ep.y < cfg.detector.tpcYminMm || ep.y > cfg.detector.tpcYmaxMm) {
                        // Electron drifted outside TPC bounds; skip
                        continue;
                    }

                    anData.z.push_back(static_cast<float>(ep.z));
                    anData.y.push_back(static_cast<float>(ep.y));
                    anData.t.push_back(static_cast<float>(ep.t));
                    anData.status.push_back(ep.status);
                    if (ep.status == 0) ++anData.nAnodeHits;

                    // Directly-hit pad (before charge spreading to neighbours)
                    int eramId = -1;
                    int padY = -1;
                    int padZ = -1;
                    if (hat.GetPadIndices(ep.y, ep.z, eramId, padY, padZ) &&
                        !hat.GetERAM(eramId).isQuartzWindow) {
                        anData.eramId.push_back(eramId);
                        anData.padZ.push_back(padZ);
                        anData.padY.push_back(padY);
                    } else {
                        anData.eramId.push_back(-1);
                        anData.padZ.push_back(-1);
                        anData.padY.push_back(-1);
                    }


                }
            }

            writer->WriteEvent(anData, generator->GetTruePositions(), generator->GetTrueDirections());

            if (waveformBuilder) {
                auto wfData = waveformBuilder->Build(ev, anData);
                writer->WriteEvent(wfData, generator->GetTruePositions(), generator->GetTrueDirections());
            }
        }
    }

    // ------------------------------------------------------------------
    // 7. Cleanup
    // ------------------------------------------------------------------
    std::cout << "\n[tpcmc] Done. Writing output to " << cfg.run.outputFile << "\n";
    generator->Finalize();
    writer->Finalize();

    return 0;
}
