<?php

namespace Drupal\redhen_connection\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Connection entities.
 */
class ConnectionViewsData extends EntityViewsData implements EntityViewsDataInterface {
  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    // Unset the default entity relationships.
    // It does not work properly, the target type it is not defined.
    unset($data['redhen_connection']['endpoint_1']['relationship']);
    unset($data['redhen_connection']['endpoint_2']['relationship']);

    return $data;
  }
}
