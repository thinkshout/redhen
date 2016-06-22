<?php

namespace Drupal\redhen\Event;

/**
 * Contains all events thrown while handling RedHen entities.
 */
final class EntityEvents {

  /**
   * The name of the event triggered when a RedHen entity is made active.
   *
   * This event allows modules to react whenever a RedHen entity (contact, org,
   * connection) that is inactive is made active.
   *
   * @Event
   *
   *
   * @var string
   */
  const ACTIVE = 'redhen.active';

  /**
   * The name of the event triggered when a RedHen entity is made inactive.
   *
   * This event allows modules to react whenever a RedHen entity (contact, org,
   * connection) that is active is made inactive.
   *
   * @Event
   *
   *
   * @var string
   */
  const INACTIVE = 'redhen.inactive';

}