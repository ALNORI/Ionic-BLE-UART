import { NgModule } from '@angular/core';
import { IonicPageModule } from 'ionic-angular';
import { DetailPage } from './detail';
import { ButtonStateDescriptionPipe } from '../../pipes/button-state-description/button-state-description';

@NgModule({
  declarations: [
    DetailPage,
    ButtonStateDescriptionPipe
  ],
  imports: [
    IonicPageModule.forChild(DetailPage),
  ],
})
export class DetailPageModule {}
