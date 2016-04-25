<?php

/**
 * @file
 * Contains \Drupal\redhen_org\Entity\Org.
 */

namespace Drupal\redhen_org\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for Org entities.
 */
class OrgViewsData extends EntityViewsData implements EntityViewsDataInterface {
  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['redhen_org']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('Org ID'),
      'help' => $this->t('The Org ID.'),
    );

    return $data;
  }

}
