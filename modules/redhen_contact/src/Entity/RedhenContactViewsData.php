<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\Entity\RedhenContact.
 */

namespace Drupal\redhen_contact\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Contact entities.
 */
class RedhenContactViewsData extends EntityViewsData implements EntityViewsDataInterface {
  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['redhen_contact']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Contact'),
      'help' => $this->t('The Contact ID.'),
    );

    return $data;
  }

}
