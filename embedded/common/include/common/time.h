#pragma once

#include "common/rusttypes.h"

namespace common {
    namespace time {
        class us;

        class ms {
            u64 m_value;

          public:
            explicit ms();
            explicit ms(u64 value);
            explicit ms(const us &value);
            operator us() const;
            u64 value() const;
            void add(const ms &other);
            ms since(const ms &older) const;
        };

        class us {
            u64 m_value;

          public:
            explicit us();
            explicit us(u64 value);
            explicit us(const ms &value);
            operator ms() const;
            u64 value() const;
            void add(const us &other);
            us since(const us &older) const;
        };

        // set with an external time source.
        void set_current_epoch(const ms &d);
        ms since_epoch();
        ms since_reset();
        us since_reset_us();
        ms elapsed_since(const ms &t0);
        us elapsed_since(const us ot0);

        void delay(const ms &delay);

        void simulate(const us &delay);
    };
};
