<?php

/**
 * @file
 * Contains \Drupal\redhen_contact\RedhenContactListBuilder.
 */

namespace Drupal\redhen_contact;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Routing\LinkGeneratorTrait;
use Drupal\Core\Url;

/**
 * Defines a class to build a listing of Contact entities.
 *
 * @ingroup redhen_contact
 */
class RedhenContactListBuilder extends EntityListBuilder {
  use LinkGeneratorTrait;
  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['type'] = $this->t('Type');
    $header['name'] = $this->t('Name');
    $header['email'] = $this->t('Email');
    $header['status'] = $this->t('Status');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\redhen_contact\Entity\RedhenContact */
    $row['type'] = $entity->getType();
    $row['name'] = $entity->link();
    $row['email'] = $entity->getEmail();
    $row['status'] = $entity->isActive() ? $this->t('Active') : $this->t('Inactive');
    return $row + parent::buildRow($entity);
  }

}
